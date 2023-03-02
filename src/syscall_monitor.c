/* See LICENSE file for copyright and license details. */
#include "syscall.h"

#include "bsp/timer.h"
#include "cap.h"
#include "common.h"
#include "consts.h"
#include "csr.h"
#include "sched.h"
#include "trap.h"


struct proc *syscall_msuspend(struct proc *proc, uint64_t mon_idx, uint64_t pid)
{
	cnode_handle_t mon_handle = cnode_get_handle(proc->pid, mon_idx);
	union cap cap = cnode_get_cap(mon_handle);

	if (!cnode_contains(mon_handle)) {
		proc->regs[REG_A0] = EXCPT_EMPTY;
		return proc;
	}
	if (cap.type != CAPTY_MONITOR) {
		proc->regs[REG_A0] = EXCPT_UNIMPLEMENTED;
		return proc;
	}
	if (cap.monitor.free > pid || pid >= cap.monitor.end) {
		proc->regs[REG_A0] = EXCPT_MPID;
		return proc;
	}

	struct proc *other_proc = &processes[pid];
	syscall_lock();
	if (!cnode_contains(mon_handle)) {
		proc->regs[REG_A0] = EXCPT_EMPTY;
		return proc;
	}
	__sync_fetch_and_or(&other_proc->state, PSF_SUSPEND);
	syscall_unlock();
	proc->regs[REG_A0] = EXCPT_NONE;
	return proc;
}

struct proc *syscall_mresume(struct proc *proc, uint64_t mon_idx, uint64_t pid)
{
	cnode_handle_t mon_handle = cnode_get_handle(proc->pid, mon_idx);
	union cap cap = cnode_get_cap(mon_handle);

	if (!cnode_contains(mon_handle)) {
		proc->regs[REG_A0] = EXCPT_EMPTY;
		return proc;
	}
	if (cap.type != CAPTY_MONITOR) {
		proc->regs[REG_A0] = EXCPT_UNIMPLEMENTED;
		return proc;
	}
	if (cap.monitor.free > pid || pid >= cap.monitor.end) {
		proc->regs[REG_A0] = EXCPT_MPID;
		return proc;
	}

	struct proc *other_proc = &processes[pid];
	syscall_lock();
	if (!cnode_contains(mon_handle)) {
		proc->regs[REG_A0] = EXCPT_EMPTY;
		return proc;
	}
	__sync_fetch_and_and(&other_proc->state, ~PSF_SUSPEND);
	syscall_unlock();
	proc->regs[REG_A0] = EXCPT_NONE;
	return proc;
}

struct proc *syscall_mgetreg(struct proc *proc, uint64_t mon_idx, uint64_t pid, uint64_t reg)
{
	cnode_handle_t mon_handle = cnode_get_handle(proc->pid, mon_idx);
	union cap cap = cnode_get_cap(mon_handle);
	if (!cnode_contains(mon_handle)) {
		proc->regs[REG_A0] = EXCPT_EMPTY;
		return proc;
	}
	if (cap.type != CAPTY_MONITOR) {
		proc->regs[REG_A0] = EXCPT_UNIMPLEMENTED;
		return proc;
	}
	if (cap.monitor.free > pid || pid >= cap.monitor.end) {
		proc->regs[REG_A0] = EXCPT_MPID;
		return proc;
	}
	syscall_lock();
	if (!cnode_contains(mon_handle)) {
		syscall_unlock();
		proc->regs[REG_A0] = EXCPT_EMPTY;
		return proc;
	}

	struct proc *other_proc = &processes[pid];
	if (!__sync_bool_compare_and_swap(&other_proc->state, PS_SUSPENDED, PS_SUSPENDED_BUSY)) {
		syscall_unlock();
		proc->regs[REG_A0] = EXCPT_MBUSY;
		return proc;
	}
	proc->regs[REG_A1] = other_proc->regs[reg % REG_COUNT];
	__sync_fetch_and_and(&other_proc->state, ~PSF_BUSY);
	syscall_unlock();
	proc->regs[REG_A0] = EXCPT_NONE;
	return proc;
}

struct proc *syscall_msetreg(struct proc *proc, uint64_t mon_idx, uint64_t pid, uint64_t reg, uint64_t val)
{
	cnode_handle_t mon_handle = cnode_get_handle(proc->pid, mon_idx);
	union cap cap = cnode_get_cap(mon_handle);
	if (!cnode_contains(mon_handle)) {
		proc->regs[REG_A0] = EXCPT_EMPTY;
		return proc;
	}
	if (cap.type != CAPTY_MONITOR) {
		proc->regs[REG_A0] = EXCPT_UNIMPLEMENTED;
		return proc;
	}
	if (cap.monitor.free > pid || pid >= cap.monitor.end) {
		proc->regs[REG_A0] = EXCPT_MPID;
		return proc;
	}

	syscall_lock();
	if (!cnode_contains(mon_handle)) {
		syscall_unlock();
		proc->regs[REG_A0] = EXCPT_EMPTY;
		return proc;
	}
	struct proc *other_proc = &processes[pid];
	if (!__sync_bool_compare_and_swap(&other_proc->state, PS_SUSPENDED, PS_SUSPENDED_BUSY)) {
		syscall_unlock();
		proc->regs[REG_A0] = EXCPT_MBUSY;
		return proc;
	}
	other_proc->regs[reg % REG_COUNT] = val;
	__sync_fetch_and_and(&other_proc->state, ~PSF_BUSY);
	syscall_unlock();
	proc->regs[REG_A0] = EXCPT_NONE;
	return proc;
}

struct proc *syscall_mgetcap(struct proc *proc, uint64_t mon_idx, uint64_t pid, uint64_t node_idx)
{
	cnode_handle_t mon_handle = cnode_get_handle(proc->pid, mon_idx);
	union cap cap = cnode_get_cap(mon_handle);
	if (!cnode_contains(mon_handle)) {
		proc->regs[REG_A0] = EXCPT_EMPTY;
		return proc;
	}
	if (cap.type != CAPTY_MONITOR) {
		proc->regs[REG_A0] = EXCPT_UNIMPLEMENTED;
		return proc;
	}
	if (cap.monitor.free > pid || pid >= cap.monitor.end) {
		proc->regs[REG_A0] = EXCPT_MPID;
		return proc;
	}
	syscall_lock();
	if (!cnode_contains(mon_handle)) {
		syscall_unlock();
		proc->regs[REG_A0] = EXCPT_EMPTY;
		return proc;
	}

	struct proc *other_proc = &processes[pid];
	if (!__sync_bool_compare_and_swap(&other_proc->state, PS_SUSPENDED, PS_SUSPENDED_BUSY)) {
		syscall_unlock();
		proc->regs[REG_A0] = EXCPT_MBUSY;
		return proc;
	}
	cnode_handle_t node_handle = cnode_get_handle(pid, node_idx);
	union cap node_cap = cnode_get_cap(node_handle);
	proc->regs[REG_A1] = node_cap.raw;
	__sync_fetch_and_and(&other_proc->state, ~PSF_BUSY);
	syscall_unlock();
	proc->regs[REG_A0] = EXCPT_NONE;
	return proc;
}

struct proc *syscall_mtakecap(struct proc *proc, uint64_t mon_idx, uint64_t pid, uint64_t src_idx, uint64_t dst_idx)
{
	cnode_handle_t mon_handle = cnode_get_handle(proc->pid, mon_idx);
	union cap cap = cnode_get_cap(mon_handle);
	if (!cnode_contains(mon_handle)) {
		proc->regs[REG_A0] = EXCPT_EMPTY;
		return proc;
	}
	if (cap.type != CAPTY_MONITOR) {
		proc->regs[REG_A0] = EXCPT_UNIMPLEMENTED;
		return proc;
	}
	if (cap.monitor.free > pid || pid >= cap.monitor.end) {
		proc->regs[REG_A0] = EXCPT_MPID;
		return proc;
	}
	syscall_lock();
	if (!cnode_contains(mon_handle)) {
		syscall_unlock();
		proc->regs[REG_A0] = EXCPT_EMPTY;
		return proc;
	}
	struct proc *other_proc = &processes[pid];
	if (!__sync_bool_compare_and_swap(&other_proc->state, PS_SUSPENDED, PS_SUSPENDED_BUSY)) {
		syscall_unlock();
		proc->regs[REG_A0] = EXCPT_MBUSY;
		return proc;
	}
	cnode_handle_t src_handle = cnode_get_handle(pid, src_idx);
	cnode_handle_t dst_handle = cnode_get_handle(proc->pid, dst_idx);
	if (cnode_contains(src_handle) && !cnode_contains(dst_handle)) {
		union cap src_cap = cnode_get_cap(src_handle);
		cnode_move(src_handle, dst_handle);
		if (src_cap.type == CAPTY_TIME) {
			sched_update(src_cap.time.hartid, proc->pid, src_cap.time.free, src_cap.time.end);
		}
	}
	__sync_fetch_and_and(&other_proc->state, ~PSF_BUSY);
	syscall_unlock();
	proc->regs[REG_A0] = EXCPT_NONE;
	return proc;
}

struct proc *syscall_mgivecap(struct proc *proc, uint64_t mon_idx, uint64_t pid, uint64_t src_idx, uint64_t dst_idx)
{
	cnode_handle_t mon_handle = cnode_get_handle(proc->pid, mon_idx);
	union cap cap = cnode_get_cap(mon_handle);
	if (!cnode_contains(mon_handle)) {
		proc->regs[REG_A0] = EXCPT_EMPTY;
		return proc;
	}
	if (cap.type != CAPTY_MONITOR) {
		proc->regs[REG_A0] = EXCPT_UNIMPLEMENTED;
		return proc;
	}
	if (cap.monitor.free > pid || pid >= cap.monitor.end) {
		proc->regs[REG_A0] = EXCPT_MPID;
		return proc;
	}
	syscall_lock();
	if (!cnode_contains(mon_handle)) {
		syscall_unlock();
		proc->regs[REG_A0] = EXCPT_EMPTY;
		return proc;
	}

	struct proc *other_proc = &processes[pid];
	if (!__sync_bool_compare_and_swap(&other_proc->state, PS_SUSPENDED, PS_SUSPENDED_BUSY)) {
		syscall_unlock();
		proc->regs[REG_A0] = EXCPT_MBUSY;
		return proc;
	}

	cnode_handle_t src_handle = cnode_get_handle(proc->pid, src_idx);
	cnode_handle_t dst_handle = cnode_get_handle(pid, dst_idx);
	if (cnode_contains(src_handle) && !cnode_contains(dst_handle)) {
		union cap src_cap = cnode_get_cap(src_handle);
		cnode_move(src_handle, dst_handle);
		if (src_cap.type == CAPTY_TIME) {
			sched_update(src_cap.time.hartid, pid, src_cap.time.free, src_cap.time.end);
		}
	}
	__sync_fetch_and_and(&other_proc->state, ~PSF_BUSY);
	syscall_unlock();
	proc->regs[REG_A0] = EXCPT_NONE;
	return proc;
}
