/* See LICENSE file for copyright and license details. */
#include "kern/syscall.h"

#include "kern/types.h"

#include "drivers/time.h"
#include "kern/cap.h"
#include "kern/csr.h"
#include "kern/error.h"
#include "kern/sched.h"
#include "kern/trap.h"
#include "kern/types.h"

static void _sys_invalid(void);
static void _sys_get_pid(void);

typedef void (*handler_t)(reg_t a0, reg_t a1, reg_t a2, reg_t a3, reg_t a4, reg_t a5, reg_t a6);

static const handler_t handlers[] = {
	(handler_t)_sys_get_pid
};

void syscall_handler(reg_t a0, reg_t a1, reg_t a2, reg_t a3,
		reg_t a4, reg_t a5, reg_t a6, reg_t a7)
{
	if (a7 < ARRAY_SIZE(handlers))
		handlers[a7](a0, a1, a2, a3, a4, a5, a6);
	else
		_sys_invalid();
}

static bool valid_idx(reg_t idx)
{
	return idx < S3K_CAP_CNT;
}

static bool valid_slot(reg_t slot)
{
	return slot < S3K_PMP_CNT;
}

static bool valid_pid(reg_t pid)
{
	return pid < S3K_PROC_CNT;
}

static bool valid_reg(reg_t i)
{
	return i < sizeof(regs_t) / sizeof(reg_t);
}

void _sys_invalid(void)
{
	current->regs.a0 = ERR_INVARG;
}

void _sys_get_pid(void)
{
	current->regs.a0 = current->pid;
}

void syscall_reg_read(reg_t i)
{
	reg_t *regs = (reg_t*)&current->regs;
	current->regs.a0 = valid_reg(i) ? regs[i] : 0;
}

void syscall_reg_write(reg_t i, reg_t val)
{
	reg_t *regs = (reg_t*)&current->regs;
	if (valid_reg(i))
		regs[i] = val;
}

void syscall_sync(void)
{
	sched();
}

void syscall_cap_read(reg_t i)
{
	if (!valid_idx(i)) {
		current->regs.t0 = ERR_INVARG;
		return;
	}
	cidx_t c = cap_idx(current->pid, i);
	cap_data_t *buf = (cap_data_t*)&current->regs.a0;
	current->regs.t0 = cap_read(c, buf);
}

void syscall_cap_move(reg_t i, reg_t j)
{
	if (!valid_idx(i) || !valid_idx(j)) {
		current->regs.t0 = ERR_INVARG;
		return;
	}
	cidx_t src = cap_idx(current->pid, i);
	cidx_t dst = cap_idx(current->pid, j);
	current->regs.t0 = cap_move(src, dst);
}

void syscall_cap_delete(reg_t i)
{
	if (!valid_idx(i)) {
		current->regs.t0 = ERR_INVARG;
		return;
	}
	cidx_t c = cap_idx(current->pid, i);
	current->regs.t0 = cap_delete(c);
}

void syscall_cap_revoke(reg_t i)
{
	if (!valid_idx(i)) {
		current->regs.t0 = ERR_INVARG;
		return;
	}
	cidx_t c = cap_idx(current->pid, i);
	current->regs.t0 = cap_revoke(c);
}

void syscall_cap_derive(reg_t i, reg_t j, reg_t cdata)
{
	if (!valid_idx(i) || !valid_idx(j)) {
		current->regs.t0 = ERR_INVARG;
		return;
	}
	cidx_t src = cap_idx(current->pid, i);
	cidx_t dst = cap_idx(current->pid, j);
	current->regs.t0 = cap_derive(src, dst, (cap_data_t){.raw = cdata});
}

//err_t validate_pmp_load(const sys_args_t *args)
//{
//	if (!valid_idx(args->pmp_load.idx))
//		return ERR_INVALID_INDEX;
//	if (!valid_slot(args->pmp_load.slot))
//		return ERR_INVALID_SLOT;
//	return SUCCESS;
//}
//
//proc_t *handle_pmp_load(proc_t *const p, const sys_args_t *args)
//{
//	cidx_t pmp = ctable_get(p->pid, args->pmp_load.idx);
//	p->regs[REG_T0] = cap_pmp_load(pmp, args->pmp_load.slot);
//	return p;
//}
//
//err_t validate_pmp_unload(const sys_args_t *args)
//{
//	if (!valid_idx(args->pmp_unload.idx))
//		return ERR_INVALID_INDEX;
//	return SUCCESS;
//}
//
//proc_t *handle_pmp_unload(proc_t *const p, const sys_args_t *args)
//{
//	cidx_t pmp = ctable_get(p->pid, args->pmp_unload.idx);
//	p->regs[REG_T0] = cap_pmp_unload(pmp);
//	return p;
//}
//
//err_t validate_mon_suspend(const sys_args_t *args)
//{
//	if (!valid_idx(args->mon_state.mon_idx))
//		return ERR_INVALID_INDEX;
//	if (!valid_pid(args->mon_state.pid))
//		return ERR_INVALID_PID;
//	return SUCCESS;
//}
//
//proc_t *handle_mon_suspend(proc_t *const p, const sys_args_t *args)
//{
//	cidx_t mon = ctable_get(p->pid, args->mon_state.mon_idx);
//	p->regs[REG_T0] = cap_monitor_suspend(mon, args->mon_state.pid);
//	return p;
//}
//
//err_t validate_mon_resume(const sys_args_t *args)
//{
//	if (!valid_idx(args->mon_state.mon_idx))
//		return ERR_INVALID_INDEX;
//	if (!valid_pid(args->mon_state.pid))
//		return ERR_INVALID_PID;
//	return SUCCESS;
//}
//
//proc_t *handle_mon_resume(proc_t *const p, const sys_args_t *args)
//{
//	cidx_t mon = ctable_get(p->pid, args->mon_state.mon_idx);
//	p->regs[REG_T0] = cap_monitor_resume(mon, args->mon_state.pid);
//	return p;
//}
//
//err_t validate_mon_state_get(const sys_args_t *args)
//{
//	if (!valid_idx(args->mon_state.mon_idx))
//		return ERR_INVALID_INDEX;
//	if (!valid_pid(args->mon_state.pid))
//		return ERR_INVALID_PID;
//	return SUCCESS;
//}
//
//proc_t *handle_mon_state_get(proc_t *const p, const sys_args_t *args)
//{
//	cidx_t mon = ctable_get(p->pid, args->mon_state.mon_idx);
//	p->regs[REG_T0] = cap_monitor_state_get(
//	    mon, args->mon_state.pid, (proc_state_t *)&p->regs[REG_A0]);
//	return p;
//}
//
//err_t validate_mon_yield(const sys_args_t *args)
//{
//	if (!valid_idx(args->mon_state.mon_idx))
//		return ERR_INVALID_INDEX;
//	if (!valid_pid(args->mon_state.pid))
//		return ERR_INVALID_PID;
//	return SUCCESS;
//}
//
//proc_t *handle_mon_yield(proc_t *const p, const sys_args_t *args)
//{
//	cidx_t mon = ctable_get(p->pid, args->mon_state.mon_idx);
//	proc_t *next = p;
//	p->regs[REG_T0] = cap_monitor_yield(mon, args->mon_state.pid, &next);
//	return next;
//}
//
//err_t validate_mon_reg_read(const sys_args_t *args)
//{
//	if (!valid_idx(args->mon_reg_read.mon_idx))
//		return ERR_INVALID_INDEX;
//	if (!valid_pid(args->mon_reg_read.pid))
//		return ERR_INVALID_PID;
//	if (!valid_reg(args->mon_reg_read.reg))
//		return ERR_INVALID_REGISTER;
//	return SUCCESS;
//}
//
//proc_t *handle_mon_reg_read(proc_t *const p, const sys_args_t *args)
//{
//	cidx_t mon = ctable_get(p->pid, args->mon_reg_read.mon_idx);
//	p->regs[REG_T0] = cap_monitor_reg_read(mon, args->mon_reg_read.pid,
//					       args->mon_reg_read.reg,
//					       &p->regs[REG_A0]);
//	return p;
//}
//
//err_t validate_mon_reg_write(const sys_args_t *args)
//{
//	if (!valid_idx(args->mon_reg_write.mon_idx))
//		return ERR_INVALID_INDEX;
//	if (!valid_pid(args->mon_reg_write.pid))
//		return ERR_INVALID_PID;
//	if (!valid_reg(args->mon_reg_write.reg))
//		return ERR_INVALID_REGISTER;
//	return SUCCESS;
//}
//
//proc_t *handle_mon_reg_write(proc_t *const p, const sys_args_t *args)
//{
//	cidx_t mon = ctable_get(p->pid, args->mon_reg_write.mon_idx);
//	p->regs[REG_T0] = cap_monitor_reg_write(mon, args->mon_reg_write.pid,
//						args->mon_reg_write.reg,
//						args->mon_reg_write.val);
//	return p;
//}
//
//err_t validate_mon_cap_read(const sys_args_t *args)
//{
//	if (!valid_idx(args->mon_cap_read.mon_idx))
//		return ERR_INVALID_INDEX;
//	if (!valid_pid(args->mon_cap_read.pid))
//		return ERR_INVALID_PID;
//	if (!valid_idx(args->mon_cap_read.idx))
//		return ERR_INVALID_INDEX;
//	return SUCCESS;
//}
//
//proc_t *handle_mon_cap_read(proc_t *const p, const sys_args_t *args)
//{
//	cidx_t mon = ctable_get(p->pid, args->mon_cap_read.mon_idx);
//	cidx_t src = ctable_get(args->mon_cap_read.pid, args->mon_cap_read.idx);
//	p->regs[REG_T0]
//	    = cap_monitor_cap_read(mon, src, (cap_t *)&p->regs[REG_A0]);
//	return p;
//}
//
//err_t validate_mon_cap_move(const sys_args_t *args)
//{
//	if (!valid_idx(args->mon_cap_move.mon_idx))
//		return ERR_INVALID_INDEX;
//	if (!valid_pid(args->mon_cap_move.src_pid))
//		return ERR_INVALID_PID;
//	if (!valid_idx(args->mon_cap_move.src_idx))
//		return ERR_INVALID_INDEX;
//	if (!valid_pid(args->mon_cap_move.dst_pid))
//		return ERR_INVALID_PID;
//	if (!valid_idx(args->mon_cap_move.dst_idx))
//		return ERR_INVALID_INDEX;
//	return SUCCESS;
//}
//
//proc_t *handle_mon_cap_move(proc_t *const p, const sys_args_t *args)
//{
//	cidx_t mon = ctable_get(p->pid, args->mon_cap_move.mon_idx);
//	cidx_t src = ctable_get(args->mon_cap_move.src_pid,
//			       args->mon_cap_move.src_idx);
//	cidx_t dst = ctable_get(args->mon_cap_move.dst_pid,
//			       args->mon_cap_move.dst_idx);
//	p->regs[REG_T0] = cap_monitor_cap_move(mon, src, dst);
//	return p;
//}
//
//err_t validate_mon_pmp_load(const sys_args_t *args)
//{
//	if (!valid_idx(args->mon_pmp_load.mon_idx))
//		return ERR_INVALID_INDEX;
//	if (!valid_pid(args->mon_pmp_load.pid))
//		return ERR_INVALID_PID;
//	if (!valid_idx(args->mon_pmp_load.idx))
//		return ERR_INVALID_INDEX;
//	if (!valid_pid(args->mon_pmp_load.slot))
//		return ERR_INVALID_PID;
//	return SUCCESS;
//}
//
//proc_t *handle_mon_pmp_load(proc_t *const p, const sys_args_t *args)
//{
//	cidx_t mon = ctable_get(p->pid, args->mon_pmp_load.mon_idx);
//	cidx_t pmp = ctable_get(args->mon_pmp_load.pid, args->mon_pmp_load.idx);
//	p->regs[REG_T0]
//	    = cap_monitor_pmp_load(mon, pmp, args->mon_pmp_load.slot);
//	return p;
//}
//
//err_t validate_mon_pmp_unload(const sys_args_t *args)
//{
//	if (!valid_idx(args->mon_pmp_unload.mon_idx))
//		return ERR_INVALID_INDEX;
//	if (!valid_pid(args->mon_pmp_unload.pid))
//		return ERR_INVALID_PID;
//	if (!valid_idx(args->mon_pmp_unload.idx))
//		return ERR_INVALID_INDEX;
//	return SUCCESS;
//}
//
//proc_t *handle_mon_pmp_unload(proc_t *const p, const sys_args_t *args)
//{
//	cidx_t mon = ctable_get(p->pid, args->mon_pmp_unload.mon_idx);
//	cidx_t pmp
//	    = ctable_get(args->mon_pmp_unload.pid, args->mon_pmp_unload.idx);
//	p->regs[REG_T0] = cap_monitor_pmp_unload(mon, pmp);
//	return p;
//}
//
//err_t validate_sock_send(const sys_args_t *args)
//{
//	if (!valid_idx(args->sock.sock_idx))
//		return ERR_INVALID_INDEX;
//	if (!valid_idx(args->sock.cap_idx))
//		return ERR_INVALID_INDEX;
//	return SUCCESS;
//}
//
//proc_t *handle_sock_send(proc_t *const p, const sys_args_t *args)
//{
//	cidx_t sock = ctable_get(p->pid, args->sock.sock_idx);
//	const ipc_msg_t msg = {
//	    .cap_buf = ctable_get(p->pid, args->sock.cap_idx),
//	    .send_cap = args->sock.send_cap,
//	    .data = {args->sock.data[0], args->sock.data[1], args->sock.data[2],
//		     args->sock.data[3]},
//	};
//	proc_t *next = p;
//	p->regs[REG_T0] = cap_sock_send(sock, &msg, &next);
//	return next;
//}
//
//err_t validate_sock_recv(const sys_args_t *args)
//{
//	if (!valid_idx(args->sock.sock_idx))
//		return ERR_INVALID_INDEX;
//	if (!valid_idx(args->sock.cap_idx))
//		return ERR_INVALID_INDEX;
//	return SUCCESS;
//}
//
//proc_t *handle_sock_recv(proc_t *const p, const sys_args_t *args)
//{
//	cidx_t sock = ctable_get(p->pid, args->sock.sock_idx);
//	cidx_t cap_buf = ctable_get(p->pid, args->sock.cap_idx);
//	proc_t *next = p;
//	p->regs[REG_T0] = cap_sock_recv(sock, cap_buf, &next);
//	return next;
//}
//
//err_t validate_sock_sendrecv(const sys_args_t *args)
//{
//	if (!valid_idx(args->sock.sock_idx))
//		return ERR_INVALID_INDEX;
//	if (!valid_idx(args->sock.cap_idx))
//		return ERR_INVALID_INDEX;
//	return SUCCESS;
//}
//
//proc_t *handle_sock_sendrecv(proc_t *const p, const sys_args_t *args)
//{
//	cidx_t sock = ctable_get(p->pid, args->sock.sock_idx);
//	const ipc_msg_t msg = {
//	    .cap_buf = ctable_get(p->pid, args->sock.cap_idx),
//	    .send_cap = args->sock.send_cap,
//	    .data = {args->sock.data[0], args->sock.data[1], args->sock.data[2],
//		     args->sock.data[3]},
//	};
//	proc_t *next = p;
//	p->regs[REG_T0] = cap_sock_sendrecv(sock, &msg, &next);
//	return next;
//}
