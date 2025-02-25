/* See LICENSE file for copyright and license details. */
#include "syscall.h"

#include "cap/ipc.h"
#include "cap/lock.h"
#include "cap/monitor.h"
#include "cap/ops.h"
#include "cap/pmp.h"
#include "cap/table.h"
#include "cap/types.h"
#include "cap/util.h"
#include "csr.h"
#include "altc/time.h"
#include "error.h"
#include "kernel.h"
#include "sched.h"
#include "trap.h"

#include <stdbool.h>

#define ARGS 8

static inline err_t validate_get_info(const sys_args_t *);
static inline err_t validate_reg_read(const sys_args_t *);
static inline err_t validate_reg_write(const sys_args_t *);
static inline err_t validate_sync(const sys_args_t *);
static inline err_t validate_sleep(const sys_args_t *);
static inline err_t validate_cap_read(const sys_args_t *);
static inline err_t validate_cap_move(const sys_args_t *);
static inline err_t validate_cap_delete(const sys_args_t *);
static inline err_t validate_cap_revoke(const sys_args_t *);
static inline err_t validate_cap_derive(const sys_args_t *);
static inline err_t validate_pmp_load(const sys_args_t *);
static inline err_t validate_pmp_unload(const sys_args_t *);
static inline err_t validate_mon_suspend(const sys_args_t *);
static inline err_t validate_mon_resume(const sys_args_t *);
static inline err_t validate_mon_state_get(const sys_args_t *);
static inline err_t validate_mon_yield(const sys_args_t *);
static inline err_t validate_mon_reg_read(const sys_args_t *);
static inline err_t validate_mon_reg_write(const sys_args_t *);
static inline err_t validate_mon_cap_read(const sys_args_t *);
static inline err_t validate_mon_cap_move(const sys_args_t *);
static inline err_t validate_mon_pmp_load(const sys_args_t *);
static inline err_t validate_mon_pmp_unload(const sys_args_t *);
static inline err_t validate_sock_send(const sys_args_t *);
static inline err_t validate_sock_recv(const sys_args_t *);
static inline err_t validate_sock_sendrecv(const sys_args_t *);

static proc_t *handle_get_info(proc_t *const, const sys_args_t *);
static proc_t *handle_reg_read(proc_t *const, const sys_args_t *);
static proc_t *handle_reg_write(proc_t *const, const sys_args_t *);
static proc_t *handle_sync(proc_t *const, const sys_args_t *);
static proc_t *handle_sleep(proc_t *const, const sys_args_t *);
static proc_t *handle_cap_read(proc_t *const, const sys_args_t *);
static proc_t *handle_cap_move(proc_t *const, const sys_args_t *);
static proc_t *handle_cap_delete(proc_t *const, const sys_args_t *);
static proc_t *handle_cap_revoke(proc_t *const, const sys_args_t *);
static proc_t *handle_cap_derive(proc_t *const, const sys_args_t *);
static proc_t *handle_pmp_load(proc_t *const, const sys_args_t *);
static proc_t *handle_pmp_unload(proc_t *const, const sys_args_t *);
static proc_t *handle_mon_suspend(proc_t *const, const sys_args_t *);
static proc_t *handle_mon_resume(proc_t *const, const sys_args_t *);
static proc_t *handle_mon_state_get(proc_t *const, const sys_args_t *);
static proc_t *handle_mon_yield(proc_t *const, const sys_args_t *);
static proc_t *handle_mon_reg_read(proc_t *const, const sys_args_t *);
static proc_t *handle_mon_reg_write(proc_t *const, const sys_args_t *);
static proc_t *handle_mon_cap_read(proc_t *const, const sys_args_t *);
static proc_t *handle_mon_cap_move(proc_t *const, const sys_args_t *);
static proc_t *handle_mon_pmp_load(proc_t *const, const sys_args_t *);
static proc_t *handle_mon_pmp_unload(proc_t *const, const sys_args_t *);
static proc_t *handle_sock_send(proc_t *const, const sys_args_t *);
static proc_t *handle_sock_recv(proc_t *const, const sys_args_t *);
static proc_t *handle_sock_sendrecv(proc_t *const, const sys_args_t *);

typedef proc_t *(*handler_t)(proc_t *const, const sys_args_t *);
typedef err_t (*validator_t)(const sys_args_t *);

handler_t handlers[] = {
    handle_get_info,	   handle_reg_read,	handle_reg_write,
    handle_sync,	   handle_sleep,	handle_cap_read,
    handle_cap_move,	   handle_cap_delete,	handle_cap_revoke,
    handle_cap_derive,	   handle_pmp_load,	handle_pmp_unload,
    handle_mon_suspend,	   handle_mon_resume,	handle_mon_state_get,
    handle_mon_yield,	   handle_mon_reg_read, handle_mon_reg_write,
    handle_mon_cap_read,   handle_mon_cap_move, handle_mon_pmp_load,
    handle_mon_pmp_unload, handle_sock_send,	handle_sock_recv,
    handle_sock_sendrecv,
};

validator_t validators[] = {
    validate_get_info,	     validate_reg_read,	    validate_reg_write,
    validate_sync,	     validate_sleep,	    validate_cap_read,
    validate_cap_move,	     validate_cap_delete,   validate_cap_revoke,
    validate_cap_derive,     validate_pmp_load,	    validate_pmp_unload,
    validate_mon_suspend,    validate_mon_resume,   validate_mon_state_get,
    validate_mon_yield,	     validate_mon_reg_read, validate_mon_reg_write,
    validate_mon_cap_read,   validate_mon_cap_move, validate_mon_pmp_load,
    validate_mon_pmp_unload, validate_sock_send,    validate_sock_recv,
    validate_sock_sendrecv,
};

proc_t *syscall_handler(proc_t *proc)
{
	// System call arguments.
	const sys_args_t *args = (sys_args_t *)&proc->regs[REG_A0];
	uint64_t call = proc->regs[REG_T0];

	// Validate system call arguments.
	err_t err = ERR_INVALID_SYSCALL;
	if (call < ARRAY_SIZE(validators))
		err = validators[call](args);

#ifndef SMP /* Single core */
	if (err) {
		// Increment PC
		proc->regs[REG_PC] += 4;
		proc->regs[REG_T0] = err;
	} else if (kernel_preempt()) {
		proc = NULL;
	} else {
		proc->regs[REG_PC] += 4;
		proc = handlers[call](proc, args);
	}
#else /* Multicore */
	if (err) {
		// Invalid parameters
		proc->regs[REG_PC] += 4;
		proc->regs[REG_T0] = err;
	} else if (kernel_preempt()) {
		// Kernel preemption
		proc = NULL;
	} else if (call < SYS_CAP_MOVE) {
		// These system calls do not require a lock
		proc->regs[REG_PC] += 4;
		proc = handlers[call](proc, args);
	} else if (cap_lock_acquire()) {
		// These system calls requires a lock
		proc->regs[REG_PC] += 4;
		proc = handlers[call](proc, args);
		cap_lock_release();
	} else {
		// Lock acquire failed due to preemption
		proc->regs[REG_PC] += 4;
		proc->regs[REG_T0] = ERR_PREEMPTED;
		proc = NULL;
	}
#endif
	return proc;
}

static bool valid_idx(cidx_t idx)
{
	return idx < NCAP;
}

static bool valid_slot(pmp_slot_t slot)
{
	return slot < NPMP;
}

static bool valid_pid(pid_t pid)
{
	return pid < NPROC;
}

static bool valid_reg(reg_t reg)
{
	return reg < REG_CNT;
}

err_t validate_get_info(const sys_args_t *args)
{
	return SUCCESS;
}

proc_t *handle_get_info(proc_t *const p, const sys_args_t *args)
{
	switch (args->get_info.info) {
	case 0:
		p->regs[REG_A0] = p->pid;
		break;
	case 1:
		p->regs[REG_A0] = time_get();
		break;
	case 2:
		p->regs[REG_A0] = timeout_get(csrr(mhartid));
		break;
	default:
		p->regs[REG_A0] = 0;
	}
	p->regs[REG_T0] = SUCCESS;
	return p;
}

err_t validate_reg_read(const sys_args_t *args)
{
	if (!valid_reg(args->reg_read.reg))
		return ERR_INVALID_REGISTER;
	return SUCCESS;
}

proc_t *handle_reg_read(proc_t *const p, const sys_args_t *args)
{
	p->regs[REG_T0] = SUCCESS;
	p->regs[REG_A0] = p->regs[args->reg_read.reg];
	return p;
}

err_t validate_reg_write(const sys_args_t *args)
{
	if (!valid_reg(args->reg_write.reg))
		return ERR_INVALID_REGISTER;
	return SUCCESS;
}

proc_t *handle_reg_write(proc_t *const p, const sys_args_t *args)
{
	p->regs[REG_T0] = SUCCESS;
	p->regs[args->reg_write.reg] = args->reg_write.val;
	return p;
}

err_t validate_sync(const sys_args_t *args)
{
	return SUCCESS;
}

proc_t *handle_sync(proc_t *const p, const sys_args_t *args)
{
	// Full sync invokes scheduler,
	// otherwise only update memory.
	if (args->sync.full) {
		p->timeout = 0;
		return NULL;
	}
	proc_pmp_sync(p);
	return p;
}

err_t validate_sleep(const sys_args_t *args)
{
	return SUCCESS;
}

proc_t *handle_sleep(proc_t *const p, const sys_args_t *args)
{
	p->regs[REG_T0] = SUCCESS;
	if (args->sleep.time)
		p->timeout = args->sleep.time;
	return NULL;
}

err_t validate_cap_read(const sys_args_t *args)
{
	if (!valid_idx(args->cap_read.idx))
		return ERR_INVALID_INDEX;
	return SUCCESS;
}

proc_t *handle_cap_read(proc_t *const p, const sys_args_t *args)
{
	cte_t c = ctable_get(p->pid, args->cap_read.idx);
	p->regs[REG_T0] = cap_read(c, (cap_t *)&p->regs[REG_A0]);
	return p;
}

err_t validate_cap_move(const sys_args_t *args)
{
	if (!valid_idx(args->cap_move.src_idx))
		return ERR_INVALID_INDEX;
	if (!valid_idx(args->cap_move.dst_idx))
		return ERR_INVALID_INDEX;
	return SUCCESS;
}

proc_t *handle_cap_move(proc_t *const p, const sys_args_t *args)
{
	cte_t src = ctable_get(p->pid, args->cap_move.src_idx);
	cte_t dst = ctable_get(p->pid, args->cap_move.dst_idx);
	p->regs[REG_T0] = cap_move(src, dst);
	return p;
}

err_t validate_cap_delete(const sys_args_t *args)
{
	if (!valid_idx(args->cap_delete.idx))
		return ERR_INVALID_INDEX;
	return SUCCESS;
}

proc_t *handle_cap_delete(proc_t *const p, const sys_args_t *args)
{
	cte_t c = ctable_get(p->pid, args->cap_delete.idx);
	p->regs[REG_T0] = cap_delete(c);
	return p;
}

err_t validate_cap_revoke(const sys_args_t *args)
{
	if (!valid_idx(args->cap_revoke.idx))
		return ERR_INVALID_INDEX;
	return SUCCESS;
}

proc_t *handle_cap_revoke(proc_t *const p, const sys_args_t *args)
{
	cte_t c = ctable_get(p->pid, args->cap_revoke.idx);

	p->regs[REG_T0] = cap_revoke(c);
	return p->regs[REG_T0] == ERR_PREEMPTED ? NULL : p;
}

err_t validate_cap_derive(const sys_args_t *args)
{
	if (!valid_idx(args->cap_derive.src_idx))
		return ERR_INVALID_INDEX;
	if (!valid_idx(args->cap_derive.dst_idx))
		return ERR_INVALID_INDEX;
	cap_t cap = {.raw = args->cap_derive.cap_raw};
	if (!cap_is_valid(cap))
		return ERR_INVALID_DERIVATION;
	return SUCCESS;
}

proc_t *handle_cap_derive(proc_t *const p, const sys_args_t *args)
{
	cte_t src = ctable_get(p->pid, args->cap_derive.src_idx);
	cte_t dst = ctable_get(p->pid, args->cap_derive.dst_idx);
	cap_t cap = {.raw = args->cap_derive.cap_raw};
	p->regs[REG_T0] = cap_derive(src, dst, cap);
	return p;
}

err_t validate_pmp_load(const sys_args_t *args)
{
	if (!valid_idx(args->pmp_load.idx))
		return ERR_INVALID_INDEX;
	if (!valid_slot(args->pmp_load.slot))
		return ERR_INVALID_SLOT;
	return SUCCESS;
}

proc_t *handle_pmp_load(proc_t *const p, const sys_args_t *args)
{
	cte_t pmp = ctable_get(p->pid, args->pmp_load.idx);
	p->regs[REG_T0] = cap_pmp_load(pmp, args->pmp_load.slot);
	return p;
}

err_t validate_pmp_unload(const sys_args_t *args)
{
	if (!valid_idx(args->pmp_unload.idx))
		return ERR_INVALID_INDEX;
	return SUCCESS;
}

proc_t *handle_pmp_unload(proc_t *const p, const sys_args_t *args)
{
	cte_t pmp = ctable_get(p->pid, args->pmp_unload.idx);
	p->regs[REG_T0] = cap_pmp_unload(pmp);
	return p;
}

err_t validate_mon_suspend(const sys_args_t *args)
{
	if (!valid_idx(args->mon_state.mon_idx))
		return ERR_INVALID_INDEX;
	if (!valid_pid(args->mon_state.pid))
		return ERR_INVALID_PID;
	return SUCCESS;
}

proc_t *handle_mon_suspend(proc_t *const p, const sys_args_t *args)
{
	cte_t mon = ctable_get(p->pid, args->mon_state.mon_idx);
	p->regs[REG_T0] = cap_monitor_suspend(mon, args->mon_state.pid);
	return p;
}

err_t validate_mon_resume(const sys_args_t *args)
{
	if (!valid_idx(args->mon_state.mon_idx))
		return ERR_INVALID_INDEX;
	if (!valid_pid(args->mon_state.pid))
		return ERR_INVALID_PID;
	return SUCCESS;
}

proc_t *handle_mon_resume(proc_t *const p, const sys_args_t *args)
{
	cte_t mon = ctable_get(p->pid, args->mon_state.mon_idx);
	p->regs[REG_T0] = cap_monitor_resume(mon, args->mon_state.pid);
	return p;
}

err_t validate_mon_state_get(const sys_args_t *args)
{
	if (!valid_idx(args->mon_state.mon_idx))
		return ERR_INVALID_INDEX;
	if (!valid_pid(args->mon_state.pid))
		return ERR_INVALID_PID;
	return SUCCESS;
}

proc_t *handle_mon_state_get(proc_t *const p, const sys_args_t *args)
{
	cte_t mon = ctable_get(p->pid, args->mon_state.mon_idx);
	p->regs[REG_T0] = cap_monitor_state_get(
	    mon, args->mon_state.pid, (proc_state_t *)&p->regs[REG_A0]);
	return p;
}

err_t validate_mon_yield(const sys_args_t *args)
{
	if (!valid_idx(args->mon_state.mon_idx))
		return ERR_INVALID_INDEX;
	if (!valid_pid(args->mon_state.pid))
		return ERR_INVALID_PID;
	return SUCCESS;
}

proc_t *handle_mon_yield(proc_t *const p, const sys_args_t *args)
{
	cte_t mon = ctable_get(p->pid, args->mon_state.mon_idx);
	proc_t *next = p;
	p->regs[REG_T0] = cap_monitor_yield(mon, args->mon_state.pid, &next);
	return next;
}

err_t validate_mon_reg_read(const sys_args_t *args)
{
	if (!valid_idx(args->mon_reg_read.mon_idx))
		return ERR_INVALID_INDEX;
	if (!valid_pid(args->mon_reg_read.pid))
		return ERR_INVALID_PID;
	if (!valid_reg(args->mon_reg_read.reg))
		return ERR_INVALID_REGISTER;
	return SUCCESS;
}

proc_t *handle_mon_reg_read(proc_t *const p, const sys_args_t *args)
{
	cte_t mon = ctable_get(p->pid, args->mon_reg_read.mon_idx);
	p->regs[REG_T0] = cap_monitor_reg_read(mon, args->mon_reg_read.pid,
					       args->mon_reg_read.reg,
					       &p->regs[REG_A0]);
	return p;
}

err_t validate_mon_reg_write(const sys_args_t *args)
{
	if (!valid_idx(args->mon_reg_write.mon_idx))
		return ERR_INVALID_INDEX;
	if (!valid_pid(args->mon_reg_write.pid))
		return ERR_INVALID_PID;
	if (!valid_reg(args->mon_reg_write.reg))
		return ERR_INVALID_REGISTER;
	return SUCCESS;
}

proc_t *handle_mon_reg_write(proc_t *const p, const sys_args_t *args)
{
	cte_t mon = ctable_get(p->pid, args->mon_reg_write.mon_idx);
	p->regs[REG_T0] = cap_monitor_reg_write(mon, args->mon_reg_write.pid,
						args->mon_reg_write.reg,
						args->mon_reg_write.val);
	return p;
}

err_t validate_mon_cap_read(const sys_args_t *args)
{
	if (!valid_idx(args->mon_cap_read.mon_idx))
		return ERR_INVALID_INDEX;
	if (!valid_pid(args->mon_cap_read.pid))
		return ERR_INVALID_PID;
	if (!valid_idx(args->mon_cap_read.idx))
		return ERR_INVALID_INDEX;
	return SUCCESS;
}

proc_t *handle_mon_cap_read(proc_t *const p, const sys_args_t *args)
{
	cte_t mon = ctable_get(p->pid, args->mon_cap_read.mon_idx);
	cte_t src = ctable_get(args->mon_cap_read.pid, args->mon_cap_read.idx);
	p->regs[REG_T0]
	    = cap_monitor_cap_read(mon, src, (cap_t *)&p->regs[REG_A0]);
	return p;
}

err_t validate_mon_cap_move(const sys_args_t *args)
{
	if (!valid_idx(args->mon_cap_move.mon_idx))
		return ERR_INVALID_INDEX;
	if (!valid_pid(args->mon_cap_move.src_pid))
		return ERR_INVALID_PID;
	if (!valid_idx(args->mon_cap_move.src_idx))
		return ERR_INVALID_INDEX;
	if (!valid_pid(args->mon_cap_move.dst_pid))
		return ERR_INVALID_PID;
	if (!valid_idx(args->mon_cap_move.dst_idx))
		return ERR_INVALID_INDEX;
	return SUCCESS;
}

proc_t *handle_mon_cap_move(proc_t *const p, const sys_args_t *args)
{
	cte_t mon = ctable_get(p->pid, args->mon_cap_move.mon_idx);
	cte_t src = ctable_get(args->mon_cap_move.src_pid,
			       args->mon_cap_move.src_idx);
	cte_t dst = ctable_get(args->mon_cap_move.dst_pid,
			       args->mon_cap_move.dst_idx);
	p->regs[REG_T0] = cap_monitor_cap_move(mon, src, dst);
	return p;
}

err_t validate_mon_pmp_load(const sys_args_t *args)
{
	if (!valid_idx(args->mon_pmp_load.mon_idx))
		return ERR_INVALID_INDEX;
	if (!valid_pid(args->mon_pmp_load.pid))
		return ERR_INVALID_PID;
	if (!valid_idx(args->mon_pmp_load.idx))
		return ERR_INVALID_INDEX;
	if (!valid_pid(args->mon_pmp_load.slot))
		return ERR_INVALID_PID;
	return SUCCESS;
}

proc_t *handle_mon_pmp_load(proc_t *const p, const sys_args_t *args)
{
	cte_t mon = ctable_get(p->pid, args->mon_pmp_load.mon_idx);
	cte_t pmp = ctable_get(args->mon_pmp_load.pid, args->mon_pmp_load.idx);
	p->regs[REG_T0]
	    = cap_monitor_pmp_load(mon, pmp, args->mon_pmp_load.slot);
	return p;
}

err_t validate_mon_pmp_unload(const sys_args_t *args)
{
	if (!valid_idx(args->mon_pmp_unload.mon_idx))
		return ERR_INVALID_INDEX;
	if (!valid_pid(args->mon_pmp_unload.pid))
		return ERR_INVALID_PID;
	if (!valid_idx(args->mon_pmp_unload.idx))
		return ERR_INVALID_INDEX;
	return SUCCESS;
}

proc_t *handle_mon_pmp_unload(proc_t *const p, const sys_args_t *args)
{
	cte_t mon = ctable_get(p->pid, args->mon_pmp_unload.mon_idx);
	cte_t pmp
	    = ctable_get(args->mon_pmp_unload.pid, args->mon_pmp_unload.idx);
	p->regs[REG_T0] = cap_monitor_pmp_unload(mon, pmp);
	return p;
}

err_t validate_sock_send(const sys_args_t *args)
{
	if (!valid_idx(args->sock.sock_idx))
		return ERR_INVALID_INDEX;
	if (!valid_idx(args->sock.cap_idx))
		return ERR_INVALID_INDEX;
	return SUCCESS;
}

proc_t *handle_sock_send(proc_t *const p, const sys_args_t *args)
{
	cte_t sock = ctable_get(p->pid, args->sock.sock_idx);
	const ipc_msg_t msg = {
	    .cap_buf = ctable_get(p->pid, args->sock.cap_idx),
	    .send_cap = args->sock.send_cap,
	    .data = {args->sock.data[0], args->sock.data[1], args->sock.data[2],
		     args->sock.data[3]},
	};
	proc_t *next = p;
	p->regs[REG_T0] = cap_sock_send(sock, &msg, &next);
	return next;
}

err_t validate_sock_recv(const sys_args_t *args)
{
	if (!valid_idx(args->sock.sock_idx))
		return ERR_INVALID_INDEX;
	if (!valid_idx(args->sock.cap_idx))
		return ERR_INVALID_INDEX;
	return SUCCESS;
}

proc_t *handle_sock_recv(proc_t *const p, const sys_args_t *args)
{
	cte_t sock = ctable_get(p->pid, args->sock.sock_idx);
	cte_t cap_buf = ctable_get(p->pid, args->sock.cap_idx);
	proc_t *next = p;
	p->regs[REG_T0] = cap_sock_recv(sock, cap_buf, &next);
	return next;
}

err_t validate_sock_sendrecv(const sys_args_t *args)
{
	if (!valid_idx(args->sock.sock_idx))
		return ERR_INVALID_INDEX;
	if (!valid_idx(args->sock.cap_idx))
		return ERR_INVALID_INDEX;
	return SUCCESS;
}

proc_t *handle_sock_sendrecv(proc_t *const p, const sys_args_t *args)
{
	cte_t sock = ctable_get(p->pid, args->sock.sock_idx);
	const ipc_msg_t msg = {
	    .cap_buf = ctable_get(p->pid, args->sock.cap_idx),
	    .send_cap = args->sock.send_cap,
	    .data = {args->sock.data[0], args->sock.data[1], args->sock.data[2],
		     args->sock.data[3]},
	};
	proc_t *next = p;
	p->regs[REG_T0] = cap_sock_sendrecv(sock, &msg, &next);
	return next;
}
