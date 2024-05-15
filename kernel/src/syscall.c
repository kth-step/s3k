/* See LICENSE file for copyright and license details. */
#include "syscall.h"

#include "cap_ipc.h"
#include "cap_monitor.h"
#include "cap_ops.h"
#include "cap_pmp.h"
#include "cap_table.h"
#include "cap_types.h"
#include "cap_util.h"
#include "csr.h"
#include "drivers/time.h"
#include "error.h"
#include "kernel.h"
#include "preempt.h"
#include "sched.h"
#include "trap.h"

#include <stdbool.h>

#define ARGS 8

/** True if process p should ignore ERR_PREEMPTED for system call */
static err_t validate_arguments(uint64_t call, const sys_args_t *args);
static err_t sys_get_info(proc_t *p, const sys_args_t *args, uint64_t *ret);
static err_t sys_reg_read(proc_t *p, const sys_args_t *args, uint64_t *ret);
static err_t sys_reg_write(proc_t *p, const sys_args_t *args, uint64_t *ret);
static err_t sys_sync(proc_t *p, const sys_args_t *args, uint64_t *ret);
static err_t sys_cap_read(proc_t *p, const sys_args_t *args, uint64_t *ret);
static err_t sys_cap_move(proc_t *p, const sys_args_t *args, uint64_t *ret);
static err_t sys_cap_delete(proc_t *p, const sys_args_t *args, uint64_t *ret);
static err_t sys_cap_revoke(proc_t *p, const sys_args_t *args, uint64_t *ret);
static err_t sys_cap_derive(proc_t *p, const sys_args_t *args, uint64_t *ret);
static err_t sys_pmp_load(proc_t *p, const sys_args_t *args, uint64_t *ret);
static err_t sys_pmp_unload(proc_t *p, const sys_args_t *args, uint64_t *ret);
static err_t sys_mon_suspend(proc_t *p, const sys_args_t *args, uint64_t *ret);
static err_t sys_mon_resume(proc_t *p, const sys_args_t *args, uint64_t *ret);
static err_t sys_mon_state_get(proc_t *p, const sys_args_t *args,
			       uint64_t *ret);
static err_t sys_mon_yield(proc_t *p, const sys_args_t *args, uint64_t *ret);
static err_t sys_mon_reg_read(proc_t *p, const sys_args_t *args, uint64_t *ret);
static err_t sys_mon_reg_write(proc_t *p, const sys_args_t *args,
			       uint64_t *ret);
static err_t sys_mon_cap_read(proc_t *p, const sys_args_t *args, uint64_t *ret);
static err_t sys_mon_cap_move(proc_t *p, const sys_args_t *args, uint64_t *ret);
static err_t sys_mon_pmp_load(proc_t *p, const sys_args_t *args, uint64_t *ret);
static err_t sys_mon_pmp_unload(proc_t *p, const sys_args_t *args,
				uint64_t *ret);
static err_t sys_sock_send(proc_t *p, const sys_args_t *args, uint64_t *ret);
static err_t sys_sock_recv(proc_t *p, const sys_args_t *args, uint64_t *ret);
static err_t sys_sock_sendrecv(proc_t *p, const sys_args_t *args,
			       uint64_t *ret);

typedef err_t (*sys_handler_t)(proc_t *, const sys_args_t *, uint64_t *);

sys_handler_t handlers[] = {
    sys_get_info,	sys_reg_read,	   sys_reg_write,    sys_sync,
    sys_cap_read,	sys_cap_move,	   sys_cap_delete,   sys_cap_revoke,
    sys_cap_derive,	sys_pmp_load,	   sys_pmp_unload,   sys_mon_suspend,
    sys_mon_resume,	sys_mon_state_get, sys_mon_yield,    sys_mon_reg_read,
    sys_mon_reg_write,	sys_mon_cap_read,  sys_mon_cap_move, sys_mon_pmp_load,
    sys_mon_pmp_unload, sys_sock_send,	   sys_sock_recv,    sys_sock_sendrecv,
};

void handle_syscall(proc_t *p)
{
	// System call arguments.
	const sys_args_t *args = (sys_args_t *)&p->regs[REG_A0];
	// System call number.
	uint64_t call = p->regs[REG_T0];
	// Return value.
	uint64_t ret = 0;

	// Check that the arguments of the system calls are valid.
	err_t err = validate_arguments(call, args);
	if (err)
		goto fail_lbl;

	if (preempt())
		sched(p);
	kernel_hook_sys_entry(p);

	switch (call) {
		/* System calls without initial lock */
	case SYS_GET_INFO:
	case SYS_REG_READ:
	case SYS_REG_WRITE:
	case SYS_SYNC:
	case SYS_CAP_READ:
	case SYS_CAP_REVOKE:
		err = handlers[call](p, args, &ret);
		break;
	default:
		/* System calls using an initial lock */
		if (!kernel_lock(p)) {
			/* Kernel lock fails on preemption. */
			err = ERR_PREEMPTED;
			break;
		}
		err = handlers[call](p, args, &ret);
		kernel_unlock(p);
		break;
	}

	/* Exit hook for instrumentation */
	kernel_hook_sys_exit(p);

	switch (err) {
	case YIELD: { // Yield to another process.
		p->regs[REG_PC] += 4;
		p->regs[REG_T0] = SUCCESS;
		proc_t *next = (proc_t *)ret;
		if (next == NULL)
			sched(p);
		if (next != p)
			proc_release(p);
		trap_exit(next);
		UNREACHABLE();
	}
	case ERR_SUSPENDED:
	case ERR_PREEMPTED:
		p->regs[REG_PC] += 4;
		p->regs[REG_T0] = err;
		sched(p);
		UNREACHABLE();
	default:
	fail_lbl:
		p->regs[REG_PC] += 4;
		p->regs[REG_T0] = err;
		p->regs[REG_A0] = ret;
		trap_resume(p);
		UNREACHABLE();
	}
}

static bool valid_idx(cidx_t idx)
{
	return idx < S3K_CAP_CNT;
}

static bool valid_slot(pmp_slot_t slot)
{
	return slot < S3K_PMP_CNT;
}

static bool valid_pid(pid_t pid)
{
	return pid < S3K_PROC_CNT;
}

static bool valid_reg(reg_t reg)
{
	return reg < REG_CNT;
}

err_t validate_arguments(uint64_t call, const sys_args_t *args)
{
	// Check the argument of the system call, if they are
	// within bounds.
	// Checks start from the first argument.
	switch (call) {
	case SYS_GET_INFO:
	case SYS_SYNC:
		return SUCCESS;

	case SYS_REG_READ:
	case SYS_REG_WRITE:
		if (!valid_reg(args->reg.reg))
			return ERR_INVALID_REGISTER;
		return SUCCESS;

	case SYS_CAP_READ:
	case SYS_CAP_DELETE:
	case SYS_CAP_REVOKE:
		if (!valid_idx(args->cap.idx))
			return ERR_INVALID_INDEX;
		return SUCCESS;

	case SYS_CAP_MOVE:
		if (!valid_idx(args->cap.idx))
			return ERR_INVALID_INDEX;
		if (!valid_idx(args->cap.dst_idx))
			return ERR_INVALID_INDEX;
		return SUCCESS;

	case SYS_CAP_DERIVE:
		if (!valid_idx(args->cap.idx))
			return ERR_INVALID_INDEX;
		if (!valid_idx(args->cap.dst_idx))
			return ERR_INVALID_INDEX;
		if (!cap_is_valid(args->cap.cap))
			return ERR_INVALID_DERIVATION;
		return SUCCESS;

	case SYS_PMP_LOAD:
		if (!valid_idx(args->pmp.pmp_idx))
			return ERR_INVALID_INDEX;
		if (!valid_slot(args->pmp.pmp_slot))
			return ERR_INVALID_SLOT;
		return SUCCESS;

	case SYS_PMP_UNLOAD:
		if (!valid_idx(args->pmp.pmp_idx))
			return ERR_INVALID_INDEX;
		return SUCCESS;

	case SYS_MON_SUSPEND:
	case SYS_MON_RESUME:
	case SYS_MON_STATE_GET:
	case SYS_MON_YIELD:
		if (!valid_idx(args->mon_state.mon_idx))
			return ERR_INVALID_INDEX;
		if (!valid_pid(args->mon_state.pid))
			return ERR_INVALID_PID;
		return SUCCESS;

	case SYS_MON_REG_READ:
	case SYS_MON_REG_WRITE:
		if (!valid_idx(args->mon_reg.mon_idx))
			return ERR_INVALID_INDEX;
		if (!valid_pid(args->mon_reg.pid))
			return ERR_INVALID_PID;
		if (!valid_reg(args->mon_reg.reg))
			return ERR_INVALID_REGISTER;
		return SUCCESS;

	case SYS_MON_CAP_READ:
		if (!valid_idx(args->mon_cap.mon_idx))
			return ERR_INVALID_INDEX;
		if (!valid_pid(args->mon_cap.pid))
			return ERR_INVALID_PID;
		if (!valid_idx(args->mon_cap.idx))
			return ERR_INVALID_INDEX;
		return SUCCESS;

	case SYS_MON_CAP_MOVE:
		if (!valid_idx(args->mon_cap.mon_idx))
			return ERR_INVALID_INDEX;
		if (!valid_pid(args->mon_cap.pid))
			return ERR_INVALID_PID;
		if (!valid_idx(args->mon_cap.idx))
			return ERR_INVALID_INDEX;
		if (!valid_pid(args->mon_cap.dst_pid))
			return ERR_INVALID_PID;
		if (!valid_idx(args->mon_cap.dst_idx))
			return ERR_INVALID_INDEX;
		return SUCCESS;

	case SYS_MON_PMP_LOAD:
		if (!valid_idx(args->mon_pmp.mon_idx))
			return ERR_INVALID_INDEX;
		if (!valid_pid(args->mon_pmp.pid))
			return ERR_INVALID_PID;
		if (!valid_idx(args->mon_pmp.pmp_idx))
			return ERR_INVALID_INDEX;
		if (!valid_slot(args->mon_pmp.pmp_slot))
			return ERR_INVALID_SLOT;
		return SUCCESS;

	case SYS_MON_PMP_UNLOAD:
		if (!valid_idx(args->mon_pmp.mon_idx))
			return ERR_INVALID_INDEX;
		if (!valid_pid(args->mon_pmp.pid))
			return ERR_INVALID_PID;
		if (!valid_idx(args->mon_pmp.pmp_idx))
			return ERR_INVALID_INDEX;
		return SUCCESS;

	case SYS_SOCK_SEND:
	case SYS_SOCK_SENDRECV:
		if (!valid_idx(args->sock.sock_idx))
			return ERR_INVALID_INDEX;
		if (!valid_idx(args->sock.cap_idx))
			return ERR_INVALID_INDEX;
		return SUCCESS;
	default:
		return ERR_INVALID_SYSCALL;
	}
}

err_t sys_get_info(proc_t *p, const sys_args_t *args, uint64_t *ret)
{
	switch (args->get_info.info) {
	case 0:
		*ret = p->pid;
		break;
	case 1:
		*ret = time_get();
		break;
	case 2:
		*ret = timeout_get(csrr_mhartid());
		break;
	case 3:
		*ret = kernel_wcet();
		break;
	case 4:
		*ret = kernel_wcet();
		kernel_wcet_reset();
		break;
	default:
		*ret = 0;
	}
	return SUCCESS;
}

err_t sys_reg_read(proc_t *p, const sys_args_t *args, uint64_t *ret)
{
	*ret = p->regs[args->reg.reg];
	return SUCCESS;
}

err_t sys_reg_write(proc_t *p, const sys_args_t *args, uint64_t *ret)
{
	p->regs[args->reg.reg] = args->reg.val;
	return SUCCESS;
}

err_t sys_sync(proc_t *p, const sys_args_t *args, uint64_t *ret)
{
	// Full sync invokes scheduler,
	// otherwise only update memory.
	*ret = args->sync.full ? 0 : ((uint64_t)p);
	return YIELD;
}

err_t sys_cap_read(proc_t *p, const sys_args_t *args, uint64_t *ret)
{
	cte_t c = ctable_get(p->pid, args->cap.idx);
	return cap_read(c, (cap_t *)ret);
}

err_t sys_cap_move(proc_t *p, const sys_args_t *args, uint64_t *ret)
{
	cte_t src = ctable_get(p->pid, args->cap.idx);
	cte_t dst = ctable_get(p->pid, args->cap.dst_idx);
	return cap_move(src, dst, (cap_t *)ret);
}

err_t sys_cap_delete(proc_t *p, const sys_args_t *args, uint64_t *ret)
{
	cte_t c = ctable_get(p->pid, args->cap.idx);
	return cap_delete(c);
}

err_t sys_cap_revoke(proc_t *p, const sys_args_t *args, uint64_t *ret)
{
	cte_t c = ctable_get(p->pid, args->cap.idx);
	while (1) {
		cap_t cap = cte_cap(c);
		cte_t next = cte_next(c);
		cap_t ncap = cte_cap(next);
		if (!cap.type)
			return ERR_EMPTY;
		// If ncap can not be revoked, we have no more children.
		if (!cap_is_revokable(cap, ncap))
			break;
		if (!kernel_lock(p))
			return ERR_PREEMPTED;
		// Delete (next, ncap), take its resource, and update (c, cap)
		// The delete may fail due to interference.
		// Checks cte_next(c) == next && cte_cap == ncap before
		// deleting.
		cap_reclaim(c, cap, next, ncap);
		kernel_unlock(p);
	}

	// We should reach here if we have no more children.
	if (!kernel_lock(p))
		return ERR_PREEMPTED;
	// Reset the capability at c.
	err_t err = cap_reset(c);
	kernel_unlock(p);
	return err;
}

err_t sys_cap_derive(proc_t *p, const sys_args_t *args, uint64_t *ret)
{
	cte_t src = ctable_get(p->pid, args->cap.idx);
	cte_t dst = ctable_get(p->pid, args->cap.dst_idx);
	return cap_derive(src, dst, args->cap.cap);
}

err_t sys_pmp_load(proc_t *p, const sys_args_t *args, uint64_t *ret)
{
	cte_t pmp = ctable_get(p->pid, args->pmp.pmp_idx);
	return cap_pmp_load(pmp, args->pmp.pmp_slot);
}

err_t sys_pmp_unload(proc_t *p, const sys_args_t *args, uint64_t *ret)
{
	cte_t pmp = ctable_get(p->pid, args->pmp.pmp_idx);
	return cap_pmp_unload(pmp);
}

err_t sys_mon_suspend(proc_t *p, const sys_args_t *args, uint64_t *ret)
{
	cte_t mon = ctable_get(p->pid, args->mon_state.mon_idx);
	return cap_monitor_suspend(mon, args->mon_state.pid);
}

err_t sys_mon_resume(proc_t *p, const sys_args_t *args, uint64_t *ret)
{
	cte_t mon = ctable_get(p->pid, args->mon_state.mon_idx);
	return cap_monitor_resume(mon, args->mon_state.pid);
}

err_t sys_mon_state_get(proc_t *p, const sys_args_t *args, uint64_t *ret)
{
	cte_t mon = ctable_get(p->pid, args->mon_state.mon_idx);
	return cap_monitor_state_get(mon, args->mon_state.pid,
				     (proc_state_t *)ret);
}

err_t sys_mon_yield(proc_t *p, const sys_args_t *args, uint64_t *ret)
{
	cte_t mon = ctable_get(p->pid, args->mon_state.mon_idx);
	return cap_monitor_yield(mon, args->mon_state.pid, (proc_t **)ret);
}

err_t sys_mon_reg_read(proc_t *p, const sys_args_t *args, uint64_t *ret)
{
	cte_t mon = ctable_get(p->pid, args->mon_reg.mon_idx);
	return cap_monitor_reg_read(mon, args->mon_reg.pid, args->mon_reg.reg,
				    ret);
}

err_t sys_mon_reg_write(proc_t *p, const sys_args_t *args, uint64_t *ret)
{
	cte_t mon = ctable_get(p->pid, args->mon_reg.mon_idx);
	return cap_monitor_reg_write(mon, args->mon_reg.pid, args->mon_reg.reg,
				     args->mon_reg.val);
}

err_t sys_mon_cap_read(proc_t *p, const sys_args_t *args, uint64_t *ret)
{
	cte_t mon = ctable_get(p->pid, args->mon_cap.mon_idx);
	cte_t src = ctable_get(args->mon_cap.pid, args->mon_cap.idx);
	return cap_monitor_cap_read(mon, src, (cap_t *)ret);
}

err_t sys_mon_cap_move(proc_t *p, const sys_args_t *args, uint64_t *ret)
{
	cte_t mon = ctable_get(p->pid, args->mon_cap.mon_idx);
	cte_t src = ctable_get(args->mon_cap.pid, args->mon_cap.idx);
	cte_t dst = ctable_get(args->mon_cap.dst_pid, args->mon_cap.dst_idx);
	return cap_monitor_cap_move(mon, src, dst);
}

err_t sys_mon_pmp_load(proc_t *p, const sys_args_t *args, uint64_t *ret)
{
	cte_t mon = ctable_get(p->pid, args->mon_pmp.mon_idx);
	cte_t pmp = ctable_get(args->mon_pmp.pid, args->mon_pmp.pmp_idx);
	return cap_monitor_pmp_load(mon, pmp, args->mon_pmp.pmp_slot);
}

err_t sys_mon_pmp_unload(proc_t *p, const sys_args_t *args, uint64_t *ret)
{
	cte_t mon = ctable_get(p->pid, args->mon_pmp.mon_idx);
	cte_t pmp = ctable_get(args->mon_pmp.pid, args->mon_pmp.pmp_idx);
	return cap_monitor_pmp_unload(mon, pmp);
}

err_t sys_sock_send(proc_t *p, const sys_args_t *args, uint64_t *ret)
{
	cte_t sock = ctable_get(p->pid, args->sock.sock_idx);
	const ipc_msg_t msg = {
	    .src_buf = ctable_get(p->pid, args->sock.cap_idx),
	    .send_cap = args->sock.send_cap,
	    .data = {args->sock.data[0], args->sock.data[1], args->sock.data[1],
		     args->sock.data[3]},
	};
	return cap_sock_send(sock, &msg, (proc_t **)ret);
}

err_t sys_sock_recv(proc_t *p, const sys_args_t *args, uint64_t *ret)
{
	cte_t sock = ctable_get(p->pid, args->sock.sock_idx);
	p->cap_buf = ctable_get(p->pid, args->sock.cap_idx);
	return cap_sock_recv(sock);
}

err_t sys_sock_sendrecv(proc_t *p, const sys_args_t *args, uint64_t *ret)
{
	cte_t sock = ctable_get(p->pid, args->sock.sock_idx);
	p->cap_buf = ctable_get(p->pid, args->sock.cap_idx);
	const ipc_msg_t msg = {
	    .src_buf = ctable_get(p->pid, args->sock.cap_idx),
	    .send_cap = args->sock.send_cap,
	    .data = {args->sock.data[0], args->sock.data[1], args->sock.data[1],
		     args->sock.data[3]},
	};
	return cap_sock_sendrecv(sock, &msg, (proc_t **)ret);
}
