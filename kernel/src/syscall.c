/* See LICENSE file for copyright and license details. */
#include "syscall.h"

#include "cap_ipc.h"
#include "cap_monitor.h"
#include "cap_ops.h"
#include "cap_pmp.h"
#include "cap_table.h"
#include "cap_types.h"
#include "csr.h"
#include "drivers/timer.h"
#include "error.h"
#include "kernel.h"
#include "sched.h"
#include "trap.h"

#include <stdbool.h>

#define ARGS 8

/** True if process p should ignore ERR_PREEMPTED for system call */
static err_t sys_get_info(proc_t *p, reg_t args[ARGS], reg_t *ret);
static err_t sys_reg_read(proc_t *p, reg_t args[ARGS], reg_t *ret);
static err_t sys_reg_write(proc_t *p, reg_t args[ARGS], reg_t *ret);
static err_t sys_sync(proc_t *p, reg_t args[ARGS], reg_t *ret);
static err_t sys_sync_mem(proc_t *p, reg_t args[ARGS], reg_t *ret);
static err_t sys_cap_read(proc_t *p, reg_t args[ARGS], reg_t *ret);
static err_t sys_cap_move(proc_t *p, reg_t args[ARGS], reg_t *ret);
static err_t sys_cap_delete(proc_t *p, reg_t args[ARGS], reg_t *ret);
static err_t sys_cap_revoke(proc_t *p, reg_t args[ARGS], reg_t *ret);
static err_t sys_cap_derive(proc_t *p, reg_t args[ARGS], reg_t *ret);
static err_t sys_pmp_load(proc_t *p, reg_t args[ARGS], reg_t *ret);
static err_t sys_pmp_unload(proc_t *p, reg_t args[ARGS], reg_t *ret);
static err_t sys_mon_suspend(proc_t *p, reg_t args[ARGS], reg_t *ret);
static err_t sys_mon_resume(proc_t *p, reg_t args[ARGS], reg_t *ret);
static err_t sys_mon_reg_read(proc_t *p, reg_t args[ARGS], reg_t *ret);
static err_t sys_mon_reg_write(proc_t *p, reg_t args[ARGS], reg_t *ret);
static err_t sys_mon_cap_read(proc_t *p, reg_t args[ARGS], reg_t *ret);
static err_t sys_mon_cap_move(proc_t *p, reg_t args[ARGS], reg_t *ret);
static err_t sys_mon_pmp_load(proc_t *p, reg_t args[ARGS], reg_t *ret);
static err_t sys_mon_pmp_unload(proc_t *p, reg_t args[ARGS], reg_t *ret);
static err_t sys_sock_send(proc_t *p, reg_t args[ARGS], reg_t *ret);
static err_t sys_sock_sendrecv(proc_t *p, reg_t args[ARGS], reg_t *ret);

typedef err_t (*sys_handler_t)(proc_t *, reg_t *, reg_t *);

sys_handler_t handlers[] = {
    sys_get_info,     sys_reg_read,	 sys_reg_write,	   sys_sync,
    sys_sync_mem,     sys_cap_read,	 sys_cap_move,	   sys_cap_delete,
    sys_cap_revoke,   sys_cap_derive,	 sys_pmp_load,	   sys_pmp_unload,
    sys_mon_suspend,  sys_mon_resume,	 sys_mon_reg_read, sys_mon_reg_write,
    sys_mon_cap_read, sys_mon_cap_move,	 sys_mon_pmp_load, sys_mon_pmp_unload,
    sys_sock_send,    sys_sock_sendrecv,
};

void handle_syscall(proc_t *p)
{
	// Error code.
	err_t err = ERR_INVALID_SYSCALL;
	// System call arguments.
	reg_t *args = &p->tf.a0;
	// System call number.
	reg_t call = p->tf.t0;
	// Return value.
	reg_t ret = 0;

	kernel_hook_sys_entry(p);

	p->tf.pc += 4;
	if (call < ARRAY_SIZE(handlers))
		err = handlers[call](p, args, &ret);

	kernel_hook_sys_exit(p);

	switch (err) {
	case YIELD: // Yield to another process.
		p->tf.t0 = SUCCESS;
		if (!ret)
			sched(p);
		proc_release(p);
		trap_exit((proc_t*)ret);
	case ERR_SUSPENDED:
	case ERR_PREEMPTED:
		p->tf.t0 = err;
		sched(p);
	default:
		p->tf.a0 = ret;
		p->tf.t0 = err;
		trap_resume(p);
	}
}

err_t sys_get_info(proc_t *p, reg_t args[ARGS], reg_t *ret)
{
	switch (args[0]) {
	case 0:
		*ret = p->pid;
		break;
	case 1:
		*ret = time_get();
		break;
	case 2:
		*ret = timer_get(csrr_mhartid());
		break;
	default:
		*ret = 0;
	}
	return SUCCESS;
}

err_t sys_reg_read(proc_t *p, reg_t args[ARGS], reg_t *ret)
{
	reg_t reg = args[0];
	reg_t *regs = (reg_t *)&p->tf;
	if (reg < N_REG)
		*ret = regs[reg];
	return SUCCESS;
}

err_t sys_reg_write(proc_t *p, reg_t args[ARGS], reg_t *ret)
{
	reg_t reg = args[0], val = args[1];
	reg_t *regs = (reg_t *)&p->tf;
	if (args[0] < N_REG) {
		*ret = regs[reg];
		regs[reg] = val;
	}
	return SUCCESS;
}

err_t sys_sync(proc_t *p, reg_t args[ARGS], reg_t *ret)
{
	*ret = 0;
	return YIELD;
}

err_t sys_sync_mem(proc_t *p, reg_t args[ARGS], reg_t *ret)
{
	*ret = (uint64_t)p;
	return YIELD;
}

err_t sys_cap_read(proc_t *p, reg_t args[ARGS], reg_t *ret)
{
	cte_t c = ctable_get(p->pid, args[0]);
	if (!c)
		return ERR_INVALID_INDEX;
	return cap_read(c, (cap_t *)ret);
}

err_t sys_cap_move(proc_t *p, reg_t args[ARGS], reg_t *ret)
{
	cte_t src = ctable_get(p->pid, args[0]);
	cte_t dst = ctable_get(p->pid, args[1]);
	if (!src || !dst)
		return ERR_INVALID_INDEX;

	if (!kernel_lock(p))
		return ERR_PREEMPTED;
	err_t err = cap_move(src, dst, (cap_t *)ret);
	kernel_unlock(p);
	return err;
}

err_t sys_cap_delete(proc_t *p, reg_t args[ARGS], reg_t *ret)
{
	cte_t c = ctable_get(p->pid, args[0]);
	if (!c)
		return ERR_INVALID_INDEX;

	if (!kernel_lock(p))
		return ERR_PREEMPTED;
	err_t err = cap_delete(c);
	kernel_unlock(p);
	return err;
}

err_t sys_cap_revoke(proc_t *p, reg_t args[ARGS], reg_t *ret)
{
	cte_t c = ctable_get(p->pid, args[0]);
	if (!c)
		return ERR_INVALID_INDEX;
	while (1) {
		cap_t cap = cte_cap(c);
		cte_t next = cte_next(c);
		cap_t ncap = cte_cap(next);
		if (!cap.type)
			return ERR_EMPTY;
		if (!cap_revokable(cap, ncap))
			break;
		if (!kernel_lock(p))
			return ERR_PREEMPTED;
		cap_reclaim(c, cap, next, ncap);
		kernel_unlock(p);
	}

	if (!kernel_lock(p))
		return ERR_PREEMPTED;
	err_t err = cap_reset(c);
	kernel_unlock(p);
	return err;
}

err_t sys_cap_derive(proc_t *p, reg_t args[ARGS], reg_t *ret)
{
	cte_t src = ctable_get(p->pid, args[0]);
	cte_t dst = ctable_get(p->pid, args[1]);
	if (!src || !dst)
		return ERR_INVALID_INDEX;

	cap_t new_cap = (cap_t){.raw = args[2]};
	if (!kernel_lock(p))
		return ERR_PREEMPTED;
	err_t err = cap_derive(src, dst, new_cap);
	kernel_unlock(p);
	return err;
}

err_t sys_pmp_load(proc_t *p, reg_t args[ARGS], reg_t *ret)
{
	cte_t pmp = ctable_get(p->pid, args[0]);
	if (!pmp)
		return ERR_INVALID_INDEX;

	if (args[1] >= S3K_PMP_CNT)
		return ERR_INVALID_SLOT;

	if (!kernel_lock(p))
		return ERR_PREEMPTED;
	err_t err = cap_pmp_load(pmp, args[1]);
	kernel_unlock(p);
	return err;
}

err_t sys_pmp_unload(proc_t *p, reg_t args[ARGS], reg_t *ret)
{
	cte_t pmp = ctable_get(p->pid, args[0]);
	if (!pmp)
		return ERR_INVALID_INDEX;

	if (!kernel_lock(p))
		return ERR_PREEMPTED;
	err_t err = cap_pmp_unload(pmp);
	kernel_unlock(p);
	return err;
}

err_t sys_mon_suspend(proc_t *p, reg_t args[ARGS], reg_t *ret)
{
	cte_t mon = ctable_get(p->pid, args[0]);
	if (!mon)
		return ERR_INVALID_INDEX;

	if (args[1] >= S3K_PROC_CNT)
		return ERR_INVALID_PID;

	if (!kernel_lock(p))
		return ERR_PREEMPTED;
	err_t err = cap_monitor_suspend(mon, args[1]);
	kernel_unlock(p);
	return err;
}

err_t sys_mon_resume(proc_t *p, reg_t args[ARGS], reg_t *ret)
{
	cte_t mon = ctable_get(p->pid, args[0]);
	if (!mon)
		return ERR_INVALID_INDEX;

	if (args[1] >= S3K_PROC_CNT)
		return ERR_INVALID_PID;

	if (!kernel_lock(p))
		return ERR_PREEMPTED;
	err_t err = cap_monitor_resume(mon, args[1]);
	kernel_unlock(p);
	return err;
}

err_t sys_mon_reg_read(proc_t *p, reg_t args[ARGS], reg_t *ret)
{
	cte_t mon = ctable_get(p->pid, args[0]);
	if (!mon)
		return ERR_INVALID_INDEX;

	if (args[1] >= S3K_PROC_CNT)
		return ERR_INVALID_PID;

	if (!kernel_lock(p))
		return ERR_PREEMPTED;
	err_t err = cap_monitor_reg_read(mon, args[1], args[2], ret);
	kernel_unlock(p);
	return err;
}

err_t sys_mon_reg_write(proc_t *p, reg_t args[ARGS], reg_t *ret)
{
	cte_t mon = ctable_get(p->pid, args[0]);
	if (!mon)
		return ERR_INVALID_INDEX;

	if (args[1] >= S3K_PROC_CNT)
		return ERR_INVALID_PID;

	if (!kernel_lock(p))
		return ERR_PREEMPTED;
	err_t err = cap_monitor_reg_write(mon, args[1], args[2], args[3], ret);
	kernel_unlock(p);
	return err;
}

err_t sys_mon_cap_read(proc_t *p, reg_t args[ARGS], reg_t *ret)
{
	cte_t mon = ctable_get(p->pid, args[0]);
	cte_t src = ctable_get(args[1], args[2]);
	if (!mon || !src)
		return ERR_INVALID_INDEX;
	if (args[1] >= S3K_PROC_CNT)
		return ERR_INVALID_PID;

	if (!kernel_lock(p))
		return ERR_PREEMPTED;
	err_t err = cap_monitor_cap_read(mon, src, (cap_t *)ret);
	kernel_unlock(p);
	return err;
}

err_t sys_mon_cap_move(proc_t *p, reg_t args[ARGS], reg_t *ret)
{
	cte_t mon = ctable_get(p->pid, args[0]);
	cte_t src = ctable_get(args[1], args[2]);
	cte_t dst = ctable_get(args[3], args[4]);
	if (!mon)
		return ERR_INVALID_INDEX;
	if (!src)
		return (args[1] >= S3K_PROC_CNT) ? ERR_INVALID_PID :
						   ERR_INVALID_INDEX;
	if (!dst)
		return (args[3] >= S3K_PROC_CNT) ? ERR_INVALID_PID :
						   ERR_INVALID_INDEX;

	if (!kernel_lock(p))
		return ERR_PREEMPTED;
	err_t err = cap_monitor_cap_move(mon, src, dst, (cap_t *)ret);
	kernel_unlock(p);
	return err;
}

err_t sys_mon_pmp_load(proc_t *p, reg_t args[ARGS], reg_t *ret)
{
	cte_t mon = ctable_get(p->pid, args[0]);
	cte_t pmp = ctable_get(args[1], args[2]);
	if (!mon)
		return ERR_INVALID_INDEX;
	if (!pmp)
		return (args[1] >= S3K_PROC_CNT) ? ERR_INVALID_PID :
						   ERR_INVALID_INDEX;
	if (args[3] >= S3K_PMP_CNT)
		return ERR_INVALID_SLOT;

	if (!kernel_lock(p))
		return ERR_PREEMPTED;
	err_t err = cap_monitor_pmp_load(mon, pmp, args[3]);
	kernel_unlock(p);
	return err;
}

err_t sys_mon_pmp_unload(proc_t *p, reg_t args[ARGS], reg_t *ret)
{
	cte_t mon = ctable_get(p->pid, args[0]);
	cte_t pmp = ctable_get(args[1], args[2]);
	if (!mon)
		return ERR_INVALID_INDEX;
	if (!pmp)
		return (args[1] >= S3K_PROC_CNT) ? ERR_INVALID_PID :
						   ERR_INVALID_INDEX;

	if (!kernel_lock(p))
		return ERR_PREEMPTED;
	err_t err = cap_monitor_pmp_unload(mon, pmp);
	kernel_unlock(p);
	return err;
}

err_t sys_sock_send(proc_t *p, reg_t args[ARGS], reg_t *ret)
{
	cte_t clnt = ctable_get(p->pid, args[0]);
	if (!clnt)
		return ERR_INVALID_INDEX;

	cte_t cap_buf = ctable_get(p->pid, args[1]);
	if (!cap_buf)
		return ERR_INVALID_INDEX;

	p->cap_buf = cap_buf;

	if (!kernel_lock(p))
		return ERR_PREEMPTED;
	err_t err = cap_sock_send(p, clnt, args[6], ret);
	kernel_unlock(p);
	return err;
}

err_t sys_sock_sendrecv(proc_t *p, reg_t args[ARGS], reg_t *ret)
{
	cte_t clnt = ctable_get(p->pid, args[0]);
	if (!clnt)
		return ERR_INVALID_INDEX;

	cte_t cap_buf = ctable_get(p->pid, args[1]);
	if (!cap_buf)
		return ERR_INVALID_INDEX;

	p->cap_buf = cap_buf;
	p->service_time = args[7];

	if (!kernel_lock(p))
		return ERR_PREEMPTED;
	err_t err = cap_sock_sendrecv(p, clnt, args[6], ret);
	kernel_unlock(p);
	return err;
}
