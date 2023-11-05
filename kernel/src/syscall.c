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
#include "sched.h"
#include "trap.h"

#include <stdbool.h>

#define ARGS 8

static err_t validate_arguments(uint64_t call, const sys_args_t *args);
static proc_t *sys_get_info(proc_t *const, const sys_args_t *);
static proc_t *sys_reg_read(proc_t *const, const sys_args_t *);
static proc_t *sys_reg_write(proc_t *const, const sys_args_t *);
static proc_t *sys_sync(proc_t *const, const sys_args_t *);
static proc_t *sys_cap_read(proc_t *const, const sys_args_t *);
static proc_t *sys_cap_move(proc_t *const, const sys_args_t *);
static proc_t *sys_cap_delete(proc_t *const, const sys_args_t *);
static proc_t *sys_cap_revoke(proc_t *const, const sys_args_t *);
static proc_t *sys_cap_derive(proc_t *const, const sys_args_t *);
static proc_t *sys_pmp_load(proc_t *const, const sys_args_t *);
static proc_t *sys_pmp_unload(proc_t *const, const sys_args_t *);
static proc_t *sys_mon_suspend(proc_t *const, const sys_args_t *);
static proc_t *sys_mon_resume(proc_t *const, const sys_args_t *);
static proc_t *sys_mon_state_get(proc_t *const, const sys_args_t *);
static proc_t *sys_mon_yield(proc_t *const, const sys_args_t *);
static proc_t *sys_mon_reg_read(proc_t *const, const sys_args_t *);
static proc_t *sys_mon_reg_write(proc_t *const, const sys_args_t *);
static proc_t *sys_mon_cap_read(proc_t *const, const sys_args_t *);
static proc_t *sys_mon_cap_move(proc_t *const, const sys_args_t *);
static proc_t *sys_mon_pmp_load(proc_t *const, const sys_args_t *);
static proc_t *sys_mon_pmp_unload(proc_t *const, const sys_args_t *);
static proc_t *sys_sock_send(proc_t *const, const sys_args_t *);
static proc_t *sys_sock_recv(proc_t *const, const sys_args_t *);
static proc_t *sys_sock_sendrecv(proc_t *const, const sys_args_t *);

typedef proc_t *(*sys_handler_t)(proc_t *const, const sys_args_t *);

sys_handler_t handlers[] = {
    sys_get_info,	sys_reg_read,	   sys_reg_write,    sys_sync,
    sys_cap_read,	sys_cap_move,	   sys_cap_delete,   sys_cap_revoke,
    sys_cap_derive,	sys_pmp_load,	   sys_pmp_unload,   sys_mon_suspend,
    sys_mon_resume,	sys_mon_state_get, sys_mon_yield,    sys_mon_reg_read,
    sys_mon_reg_write,	sys_mon_cap_read,  sys_mon_cap_move, sys_mon_pmp_load,
    sys_mon_pmp_unload, sys_sock_send,	   sys_sock_recv,    sys_sock_sendrecv,
};

proc_t *handle_syscall(proc_t *proc)
{
	// System call arguments.
	const sys_args_t *args = (sys_args_t *)&proc->regs[REG_A0];
	// System call number.
	uint64_t call = proc->regs[REG_T0];
	// Validate kernel arguments, preemptively.
	err_t err = validate_arguments(call, args);

	// Increment PC
	proc->regs[REG_PC] += 4;
	proc->regs[REG_T0] = err;
	if (!err) // If arguments valid, perform system call.
		return handlers[call](proc, args);
	return proc;
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

proc_t *sys_get_info(proc_t *const p, const sys_args_t *args)
{
	switch (args->get_info.info) {
	case 0:
		p->regs[REG_A0] = p->pid;
		break;
	case 1:
		p->regs[REG_A0] = time_get();
		break;
	case 2:
		p->regs[REG_A0] = timeout_get(csrr_mhartid());
		break;
	case 3:
		p->regs[REG_A0] = kernel_wcet();
		break;
	case 4:
		p->regs[REG_A0] = kernel_wcet();
		kernel_wcet_reset();
		break;
	default:
		p->regs[REG_A0] = 0;
	}
	return p;
}

proc_t *sys_reg_read(proc_t *const p, const sys_args_t *args)
{
	p->regs[REG_A0] = p->regs[args->reg.reg];
	return p;
}

proc_t *sys_reg_write(proc_t *const p, const sys_args_t *args)
{
	p->regs[args->reg.reg] = args->reg.val;
	return p;
}

proc_t *sys_sync(proc_t *const p, const sys_args_t *args)
{
	// Full sync invokes scheduler,
	// otherwise only update memory.
	if (args->sync.full)
		return NULL;
	proc_pmp_sync(p);
	return p;
}

proc_t *sys_cap_read(proc_t *const p, const sys_args_t *args)
{
	cte_t c = ctable_get(p->pid, args->cap.idx);
	p->regs[REG_T0] = cap_read(c, (cap_t *)&p->regs[REG_A0]);
	return p;
}

proc_t *sys_cap_move(proc_t *const p, const sys_args_t *args)
{
	cte_t src = ctable_get(p->pid, args->cap.idx);
	cte_t dst = ctable_get(p->pid, args->cap.dst_idx);
	p->regs[REG_T0] = ERR_PREEMPTED;
	if (!kernel_lock_acquire())
		return NULL;
	p->regs[REG_T0] = cap_move(src, dst, (cap_t *)&p->regs[REG_A0]);
	kernel_lock_release();
	return p;
}

proc_t *sys_cap_delete(proc_t *const p, const sys_args_t *args)
{
	cte_t c = ctable_get(p->pid, args->cap.idx);
	p->regs[REG_T0] = ERR_PREEMPTED;
	if (!kernel_lock_acquire())
		return NULL;
	p->regs[REG_T0] = cap_delete(c);
	kernel_lock_release();
	return p;
}

proc_t *sys_cap_revoke(proc_t *const p, const sys_args_t *args)
{
	cte_t c = ctable_get(p->pid, args->cap.idx);
	p->regs[REG_T0] = ERR_PREEMPTED;
	err_t err;
	do {
		if (!kernel_lock_acquire())
			return NULL;
		err = cap_revoke(c);
		kernel_lock_release();
	} while (!err);

	if (err > 0) {
		p->regs[REG_T0] = err;
		return p;
	}

	// We should reach here if we have no more children.
	if (!kernel_lock_acquire())
		return NULL;
	p->regs[REG_T0] = cap_reset(c);
	kernel_lock_release();
	return p;
}

proc_t *sys_cap_derive(proc_t *const p, const sys_args_t *args)
{
	cte_t src = ctable_get(p->pid, args->cap.idx);
	cte_t dst = ctable_get(p->pid, args->cap.dst_idx);
	p->regs[REG_T0] = ERR_PREEMPTED;
	if (!kernel_lock_acquire())
		return NULL;
	p->regs[REG_T0] = cap_derive(src, dst, args->cap.cap);
	kernel_lock_release();
	return p;
}

proc_t *sys_pmp_load(proc_t *const p, const sys_args_t *args)
{
	cte_t pmp = ctable_get(p->pid, args->pmp.pmp_idx);
	p->regs[REG_T0] = ERR_PREEMPTED;
	if (!kernel_lock_acquire())
		return NULL;
	p->regs[REG_T0] = cap_pmp_load(pmp, args->pmp.pmp_slot);
	kernel_lock_release();
	return p;
}

proc_t *sys_pmp_unload(proc_t *const p, const sys_args_t *args)
{
	cte_t pmp = ctable_get(p->pid, args->pmp.pmp_idx);
	p->regs[REG_T0] = ERR_PREEMPTED;
	if (!kernel_lock_acquire())
		return NULL;
	p->regs[REG_T0] = cap_pmp_unload(pmp);
	kernel_lock_release();
	return p;
}

proc_t *sys_mon_suspend(proc_t *const p, const sys_args_t *args)
{
	cte_t mon = ctable_get(p->pid, args->mon_state.mon_idx);
	p->regs[REG_T0] = ERR_PREEMPTED;
	if (!kernel_lock_acquire())
		return NULL;
	p->regs[REG_T0] = cap_monitor_suspend(mon, args->mon_state.pid);
	kernel_lock_release();
	return p;
}

proc_t *sys_mon_resume(proc_t *const p, const sys_args_t *args)
{
	cte_t mon = ctable_get(p->pid, args->mon_state.mon_idx);
	p->regs[REG_T0] = ERR_PREEMPTED;
	if (!kernel_lock_acquire())
		return NULL;
	p->regs[REG_T0] = cap_monitor_resume(mon, args->mon_state.pid);
	kernel_lock_release();
	return p;
}

proc_t *sys_mon_state_get(proc_t *const p, const sys_args_t *args)
{
	cte_t mon = ctable_get(p->pid, args->mon_state.mon_idx);
	p->regs[REG_T0] = ERR_PREEMPTED;
	if (!kernel_lock_acquire())
		return NULL;
	p->regs[REG_T0] = cap_monitor_state_get(
	    mon, args->mon_state.pid, (proc_state_t *)&p->regs[REG_A0]);
	kernel_lock_release();
	return p;
}

proc_t *sys_mon_yield(proc_t *const p, const sys_args_t *args)
{
	cte_t mon = ctable_get(p->pid, args->mon_state.mon_idx);
	p->regs[REG_T0] = ERR_PREEMPTED;
	proc_t *next = p;
	if (!kernel_lock_acquire())
		return NULL;
	p->regs[REG_T0] = cap_monitor_yield(mon, args->mon_state.pid, &next);
	kernel_lock_release();
	return next;
}

proc_t *sys_mon_reg_read(proc_t *const p, const sys_args_t *args)
{
	cte_t mon = ctable_get(p->pid, args->mon_reg.mon_idx);
	p->regs[REG_T0] = ERR_PREEMPTED;
	if (!kernel_lock_acquire())
		return NULL;
	p->regs[REG_T0] = cap_monitor_reg_read(
	    mon, args->mon_reg.pid, args->mon_reg.reg, &p->regs[REG_A0]);
	kernel_lock_release();
	return p;
}

proc_t *sys_mon_reg_write(proc_t *const p, const sys_args_t *args)
{
	cte_t mon = ctable_get(p->pid, args->mon_reg.mon_idx);
	p->regs[REG_T0] = ERR_PREEMPTED;
	if (!kernel_lock_acquire())
		return NULL;
	p->regs[REG_T0] = cap_monitor_reg_write(
	    mon, args->mon_reg.pid, args->mon_reg.reg, args->mon_reg.val);
	kernel_lock_release();
	return p;
}

proc_t *sys_mon_cap_read(proc_t *const p, const sys_args_t *args)
{
	cte_t mon = ctable_get(p->pid, args->mon_cap.mon_idx);
	cte_t src = ctable_get(args->mon_cap.pid, args->mon_cap.idx);
	p->regs[REG_T0] = ERR_PREEMPTED;
	if (!kernel_lock_acquire())
		return NULL;
	p->regs[REG_T0]
	    = cap_monitor_cap_read(mon, src, (cap_t *)&p->regs[REG_A0]);
	kernel_lock_release();
	return p;
}

proc_t *sys_mon_cap_move(proc_t *const p, const sys_args_t *args)
{
	cte_t mon = ctable_get(p->pid, args->mon_cap.mon_idx);
	cte_t src = ctable_get(args->mon_cap.pid, args->mon_cap.idx);
	cte_t dst = ctable_get(args->mon_cap.dst_pid, args->mon_cap.dst_idx);
	p->regs[REG_T0] = ERR_PREEMPTED;
	if (!kernel_lock_acquire())
		return NULL;
	p->regs[REG_T0] = cap_monitor_cap_move(mon, src, dst);
	kernel_lock_release();
	return p;
}

proc_t *sys_mon_pmp_load(proc_t *const p, const sys_args_t *args)
{
	cte_t mon = ctable_get(p->pid, args->mon_pmp.mon_idx);
	cte_t pmp = ctable_get(args->mon_pmp.pid, args->mon_pmp.pmp_idx);
	p->regs[REG_T0] = ERR_PREEMPTED;
	if (!kernel_lock_acquire())
		return NULL;
	p->regs[REG_T0]
	    = cap_monitor_pmp_load(mon, pmp, args->mon_pmp.pmp_slot);
	kernel_lock_release();
	return p;
}

proc_t *sys_mon_pmp_unload(proc_t *const p, const sys_args_t *args)
{
	cte_t mon = ctable_get(p->pid, args->mon_pmp.mon_idx);
	cte_t pmp = ctable_get(args->mon_pmp.pid, args->mon_pmp.pmp_idx);
	p->regs[REG_T0] = ERR_PREEMPTED;
	if (!kernel_lock_acquire())
		return NULL;
	p->regs[REG_T0] = cap_monitor_pmp_unload(mon, pmp);
	kernel_lock_release();
	return p;
}

proc_t *sys_sock_send(proc_t *const p, const sys_args_t *args)
{
	cte_t sock = ctable_get(p->pid, args->sock.sock_idx);
	const ipc_msg_t msg = {
	    .src_buf = ctable_get(p->pid, args->sock.cap_idx),
	    .send_cap = args->sock.send_cap,
	    .data = {args->sock.data[0], args->sock.data[1], args->sock.data[1],
		     args->sock.data[3]},
	};
	proc_t *next = p;
	p->regs[REG_T0] = ERR_PREEMPTED;
	if (!kernel_lock_acquire())
		return NULL;
	p->regs[REG_T0] = cap_sock_send(sock, &msg, &next);
	kernel_lock_release();
	return next;
}

proc_t *sys_sock_recv(proc_t *const p, const sys_args_t *args)
{
	cte_t sock = ctable_get(p->pid, args->sock.sock_idx);
	p->cap_buf = ctable_get(p->pid, args->sock.cap_idx);
	p->regs[REG_T0] = ERR_PREEMPTED;
	proc_t *next = p;
	if (!kernel_lock_acquire())
		return NULL;
	p->regs[REG_T0] = cap_sock_recv(sock, &next);
	kernel_lock_release();
	return next;
}

proc_t *sys_sock_sendrecv(proc_t *const p, const sys_args_t *args)
{
	cte_t sock = ctable_get(p->pid, args->sock.sock_idx);
	p->cap_buf = ctable_get(p->pid, args->sock.cap_idx);
	const ipc_msg_t msg = {
	    .src_buf = ctable_get(p->pid, args->sock.cap_idx),
	    .send_cap = args->sock.send_cap,
	    .data = {args->sock.data[0], args->sock.data[1], args->sock.data[1],
		     args->sock.data[3]},
	};
	proc_t *next = p;
	p->regs[REG_T0] = ERR_PREEMPTED;
	if (!kernel_lock_acquire())
		return NULL;
	p->regs[REG_T0] = cap_sock_sendrecv(sock, &msg, &next);
	kernel_lock_release();
	return next;
}
