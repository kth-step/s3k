/* See LICENSE file for copyright and license details. */
#include "cap.h"
#include "common.h"
#include "consts.h"
#include "csr.h"
#include "schedule.h"
#include "syscall.h"
#include "timer.h"
#include "trap.h"

extern struct proc *_listeners[NCHANNEL];

void syscall_msuspend(struct proc *proc, uint64_t mon_idx, uint64_t pid)
{
	cnode_handle_t mon_handle = cnode_get_handle(proc->pid, mon_idx);
	union cap cap = cnode_get_cap(mon_handle);

	if (!cnode_contains(mon_handle)) {
		proc->regs[REG_A0] = EXCPT_EMPTY;
		return;
	}
	if (cap.type != CAPTY_MONITOR) {
		proc->regs[REG_A0] = EXCPT_UNIMPLEMENTED;
		return;
	}
	if (cap.monitor.free > pid || pid >= cap.monitor.end) {
		proc->regs[REG_A0] = EXCPT_MPID;
		return;
	}

	struct proc *other_proc = proc_get(pid);
	syscall_lock();
	if (!cnode_contains(mon_handle)) {
		proc->regs[REG_A0] = EXCPT_EMPTY;
		return;
	}
	proc_suspend(other_proc);
	syscall_unlock();
	proc->regs[REG_A0] = EXCPT_NONE;
}

void syscall_mresume(struct proc *proc, uint64_t mon_idx, uint64_t pid)
{
	cnode_handle_t mon_handle = cnode_get_handle(proc->pid, mon_idx);
	union cap cap = cnode_get_cap(mon_handle);

	if (!cnode_contains(mon_handle)) {
		proc->regs[REG_A0] = EXCPT_EMPTY;
		return;
	}
	if (cap.type != CAPTY_MONITOR) {
		proc->regs[REG_A0] = EXCPT_UNIMPLEMENTED;
		return;
	}
	if (cap.monitor.free > pid || pid >= cap.monitor.end) {
		proc->regs[REG_A0] = EXCPT_MPID;
		return;
	}

	struct proc *other_proc = proc_get(pid);
	syscall_lock();
	if (!cnode_contains(mon_handle)) {
		proc->regs[REG_A0] = EXCPT_EMPTY;
		return;
	}
	proc_resume(other_proc);
	syscall_unlock();
	proc->regs[REG_A0] = EXCPT_NONE;
}

void syscall_mgetreg(struct proc *proc, uint64_t mon_idx, uint64_t pid,
		     uint64_t reg)
{
	cnode_handle_t mon_handle = cnode_get_handle(proc->pid, mon_idx);
	union cap cap = cnode_get_cap(mon_handle);
	if (!cnode_contains(mon_handle)) {
		proc->regs[REG_A0] = EXCPT_EMPTY;
		return;
	}
	if (cap.type != CAPTY_MONITOR) {
		proc->regs[REG_A0] = EXCPT_UNIMPLEMENTED;
		return;
	}
	if (cap.monitor.free > pid || pid >= cap.monitor.end) {
		proc->regs[REG_A0] = EXCPT_MPID;
		return;
	}
	syscall_lock();
	if (!cnode_contains(mon_handle)) {
		syscall_unlock();
		proc->regs[REG_A0] = EXCPT_EMPTY;
		return;
	}

	struct proc *other_proc = proc_get(pid);
	if (!proc_acquire(other_proc, PS_SUSPENDED)) {
		syscall_unlock();
		proc->regs[REG_A0] = EXCPT_MBUSY;
		return;
	}
	proc->regs[REG_A1] = other_proc->regs[reg % REG_COUNT];
	proc_release(other_proc);
	__sync_fetch_and_and(&other_proc->state, ~PSF_BUSY);
	syscall_unlock();
	proc->regs[REG_A0] = EXCPT_NONE;
}

void syscall_msetreg(struct proc *proc, uint64_t mon_idx, uint64_t pid,
		     uint64_t reg, uint64_t val)
{
	cnode_handle_t mon_handle = cnode_get_handle(proc->pid, mon_idx);
	union cap cap = cnode_get_cap(mon_handle);
	if (!cnode_contains(mon_handle)) {
		proc->regs[REG_A0] = EXCPT_EMPTY;
		return;
	}
	if (cap.type != CAPTY_MONITOR) {
		proc->regs[REG_A0] = EXCPT_UNIMPLEMENTED;
		return;
	}
	if (cap.monitor.free > pid || pid >= cap.monitor.end) {
		proc->regs[REG_A0] = EXCPT_MPID;
		return;
	}

	syscall_lock();
	if (!cnode_contains(mon_handle)) {
		syscall_unlock();
		proc->regs[REG_A0] = EXCPT_EMPTY;
		return;
	}
	struct proc *other_proc = proc_get(pid);
	if (!proc_acquire(other_proc, PS_SUSPENDED)) {
		syscall_unlock();
		proc->regs[REG_A0] = EXCPT_MBUSY;
		return;
	}
	other_proc->regs[reg % REG_COUNT] = val;
	proc_release(other_proc);
	syscall_unlock();
	proc->regs[REG_A0] = EXCPT_NONE;
}

void syscall_mgetcap(struct proc *proc, uint64_t mon_idx, uint64_t pid,
		     uint64_t node_idx)
{
	cnode_handle_t mon_handle = cnode_get_handle(proc->pid, mon_idx);
	union cap cap = cnode_get_cap(mon_handle);
	if (!cnode_contains(mon_handle)) {
		proc->regs[REG_A0] = EXCPT_EMPTY;
		return;
	}
	if (cap.type != CAPTY_MONITOR) {
		proc->regs[REG_A0] = EXCPT_UNIMPLEMENTED;
		return;
	}
	if (cap.monitor.free > pid || pid >= cap.monitor.end) {
		proc->regs[REG_A0] = EXCPT_MPID;
		return;
	}
	syscall_lock();
	if (!cnode_contains(mon_handle)) {
		syscall_unlock();
		proc->regs[REG_A0] = EXCPT_EMPTY;
		return;
	}

	struct proc *other_proc = proc_get(pid);
	if (!proc_acquire(other_proc, PS_SUSPENDED)) {
		syscall_unlock();
		proc->regs[REG_A0] = EXCPT_MBUSY;
		return;
	}
	cnode_handle_t node_handle = cnode_get_handle(pid, node_idx);
	proc->regs[REG_A1] = cnode_get_cap(node_handle).raw;
	proc_release(other_proc);
	syscall_unlock();
	proc->regs[REG_A0] = EXCPT_NONE;
}

void syscall_mtakecap(struct proc *proc, uint64_t mon_idx, uint64_t pid,
		      uint64_t src_idx, uint64_t dst_idx)
{
	cnode_handle_t mon_handle = cnode_get_handle(proc->pid, mon_idx);
	union cap cap = cnode_get_cap(mon_handle);
	if (!cnode_contains(mon_handle)) {
		proc->regs[REG_A0] = EXCPT_EMPTY;
		return;
	}
	if (cap.type != CAPTY_MONITOR) {
		proc->regs[REG_A0] = EXCPT_UNIMPLEMENTED;
		return;
	}
	if (cap.monitor.free > pid || pid >= cap.monitor.end) {
		proc->regs[REG_A0] = EXCPT_MPID;
		return;
	}
	syscall_lock();
	if (!cnode_contains(mon_handle)) {
		syscall_unlock();
		proc->regs[REG_A0] = EXCPT_EMPTY;
		return;
	}
	struct proc *other_proc = proc_get(pid);
	if (!proc_acquire(other_proc, PS_SUSPENDED)) {
		syscall_unlock();
		proc->regs[REG_A0] = EXCPT_MBUSY;
		return;
	}
	cnode_handle_t src_handle = cnode_get_handle(pid, src_idx);
	cnode_handle_t dst_handle = cnode_get_handle(proc->pid, dst_idx);
	if (cnode_contains(src_handle) && !cnode_contains(dst_handle)) {
		union cap src_cap = cnode_get_cap(src_handle);
		cnode_move(src_handle, dst_handle);
		if (src_cap.type == CAPTY_TIME) {
			schedule_update(src_cap.time.hartid, proc->pid,
					src_cap.time.free, src_cap.time.end);
		}
		if (src_cap.type == CAPTY_SOCKET && src_cap.socket.tag == 0) {
			_listeners[src_cap.socket.channel] = proc;
		}
	}
	proc_release(other_proc);
	syscall_unlock();
	proc->regs[REG_A0] = EXCPT_NONE;
}

void syscall_mgivecap(struct proc *proc, uint64_t mon_idx, uint64_t pid,
		      uint64_t src_idx, uint64_t dst_idx)
{
	cnode_handle_t mon_handle = cnode_get_handle(proc->pid, mon_idx);
	union cap cap = cnode_get_cap(mon_handle);
	if (!cnode_contains(mon_handle)) {
		proc->regs[REG_A0] = EXCPT_EMPTY;
		return;
	}
	if (cap.type != CAPTY_MONITOR) {
		proc->regs[REG_A0] = EXCPT_UNIMPLEMENTED;
		return;
	}
	if (cap.monitor.free > pid || pid >= cap.monitor.end) {
		proc->regs[REG_A0] = EXCPT_MPID;
		return;
	}
	syscall_lock();
	if (!cnode_contains(mon_handle)) {
		syscall_unlock();
		proc->regs[REG_A0] = EXCPT_EMPTY;
		return;
	}

	struct proc *other_proc = proc_get(pid);
	if (!proc_acquire(other_proc, PS_SUSPENDED)) {
		syscall_unlock();
		proc->regs[REG_A0] = EXCPT_MBUSY;
		return;
	}

	cnode_handle_t src_handle = cnode_get_handle(proc->pid, src_idx);
	cnode_handle_t dst_handle = cnode_get_handle(pid, dst_idx);
	if (cnode_contains(src_handle) && !cnode_contains(dst_handle)) {
		union cap src_cap = cnode_get_cap(src_handle);
		cnode_move(src_handle, dst_handle);
		if (src_cap.type == CAPTY_TIME) {
			schedule_update(src_cap.time.hartid, pid,
					src_cap.time.free, src_cap.time.end);
		}
		if (src_cap.type == CAPTY_SOCKET && src_cap.socket.tag == 0) {
			_listeners[src_cap.socket.channel] = other_proc;
		}
	}
	proc_release(other_proc);
	syscall_unlock();
	proc->regs[REG_A0] = EXCPT_NONE;
}
