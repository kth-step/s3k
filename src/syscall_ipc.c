#include "kassert.h"
#include "proc.h"
#include "schedule.h"
#include "syscall.h"

struct proc *_listeners[NCHANNEL];

void syscall_recv(struct proc *proc, uint64_t recv_idx, uint64_t cap_dest)
{
	cnode_handle_t recv_cap_handle = cnode_get_handle(proc->pid, recv_idx);
	union cap recv_cap = cnode_get_cap(recv_cap_handle);
	if (!cnode_contains(recv_cap_handle)) {
		proc->regs[REG_A0] = EXCPT_EMPTY;
		return;
	}
	if (recv_cap.type != CAPTY_SOCKET || recv_cap.socket.tag != 0) {
		proc->regs[REG_A0] = EXCPT_INVALID_CAP;
		return;
	}
	uint64_t channel = recv_cap.socket.channel;
	syscall_lock();
	if (!cnode_contains(recv_cap_handle)) {
		proc->regs[REG_A0] = EXCPT_EMPTY;
		syscall_unlock();
		return;
	}

	kassert(_listeners[channel] == proc);

	proc->regs[REG_A0] = EXCPT_PREEMPTED;
	proc->cap_dest = cap_dest;
	if (proc_ipc_wait(proc, channel)) {
		// Waiting success
		syscall_unlock();
		schedule_next();
	} else {
		// Waiting failed, process is suspended.
		syscall_unlock();
		schedule_yield(proc);
	}
}

void syscall_send(struct proc *proc, uint64_t send_idx, uint64_t msg0,
		  uint64_t msg1, uint64_t msg2, uint64_t msg3, uint64_t cap_src,
		  uint64_t yield)
{
	cnode_handle_t send_cap_handle = cnode_get_handle(proc->pid, send_idx);
	union cap send_cap = cnode_get_cap(send_cap_handle);
	if (!cnode_contains(send_cap_handle)) {
		proc->regs[REG_A0] = EXCPT_EMPTY;
		return;
	}
	if (send_cap.type != CAPTY_SOCKET || send_cap.socket.tag == 0) {
		proc->regs[REG_A0] = EXCPT_INVALID_CAP;
		return;
	}
	uint64_t channel = send_cap.socket.channel;
	struct proc *receiver = _listeners[channel];
	if (receiver == NULL || !proc_ipc_acquire(receiver, channel)) {
		proc->regs[REG_A0] = EXCPT_NO_RECEIVER;
		return;
	}

	syscall_lock();
	if (!cnode_contains(send_cap_handle)) {
		proc->regs[REG_A0] = EXCPT_EMPTY;
		syscall_unlock();
		return;
	}
	// Check if we can send a capability.
	uint64_t cap_dest = receiver->cap_dest;
	cnode_handle_t dest_handle = cnode_get_handle(receiver->pid, cap_dest);
	cnode_handle_t src_handle = cnode_get_handle(proc->pid, cap_src);
	if (cap_src != -1ull) {
		if (cnode_contains(dest_handle)
		    || !cnode_contains(src_handle)) {
			proc->regs[REG_A0] = EXCPT_SEND_CAP;
			syscall_unlock();
			return;
		}
		union cap cap = cnode_get_cap(src_handle);
		cnode_move(src_handle, dest_handle);
		if (cap.type == CAPTY_TIME) {
			schedule_update(cap.time.hartid, receiver->pid,
					cap.time.free, cap.time.end);
		}
		if (cap.type == CAPTY_SOCKET && cap.socket.tag == 0) {
			_listeners[cap.socket.channel] = receiver;
		}
	}
	receiver->regs[REG_A0] = EXCPT_NONE;
	receiver->regs[REG_A1] = msg0;
	receiver->regs[REG_A2] = msg1;
	receiver->regs[REG_A3] = msg2;
	receiver->regs[REG_A4] = msg3;
	receiver->regs[REG_A5] = send_cap.socket.tag;
	proc_release(receiver);
	proc->regs[REG_A0] = EXCPT_NONE;
	syscall_unlock();
	if (yield)
		schedule_yield(proc);
}

void syscall_sendrecv(struct proc *proc, uint64_t sendrecv_idx, uint64_t msg0,
		      uint64_t msg1, uint64_t msg2, uint64_t msg3,
		      uint64_t send_cap, uint64_t dest_cap)
{
	syscall_send(proc, sendrecv_idx >> 16, msg0, msg1, msg2, msg3, send_cap,
		     false);
	syscall_recv(proc, sendrecv_idx & 0xFFFF, dest_cap);
}
