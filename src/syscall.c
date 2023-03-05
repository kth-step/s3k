/* See LICENSE file for copyright and license details. */
#include "syscall.h"

#include "cap.h"
#include "common.h"
#include "consts.h"
#include "csr.h"
#include "schedule.h"
#include "timer.h"
#include "trap.h"

// Lock used for capability operations.
static struct ticket_lock cap_lock;

// static struct proc *listeners[NCHANNEL];

void syscall_lock(void)
{
	tl_lock(&cap_lock);
}

void syscall_unlock(void)
{
	tl_unlock(&cap_lock);
}

/*** System call handlers ***/
struct proc *syscall_getinfo(struct proc *proc, uint64_t info)
{
	switch (info) {
	case 0:
		proc->regs[REG_A0] = proc->pid;
		break;
	case 1:
		proc->regs[REG_A0] = csrr_mhartid();
		break;
	case 2:
		proc->regs[REG_A0] = time_get();
		break;
	case 3:
		proc->regs[REG_A0] = timeout_get(csrr_mhartid());
		break;
	default:
		proc->regs[REG_A0] = 0;
		break;
	}
	return proc;
}

struct proc *syscall_getreg(struct proc *proc, uint64_t regIdx)
{
	uint64_t *regs = (uint64_t *)&proc->regs;
	regIdx %= REG_COUNT;
	proc->regs[REG_A0] = regs[regIdx];
	return proc;
}

struct proc *syscall_setreg(struct proc *proc, uint64_t regIdx, uint64_t val)
{
	uint64_t *regs = (uint64_t *)&proc->regs;
	regIdx %= REG_COUNT;
	proc->regs[REG_A0] = regs[regIdx];
	regs[regIdx] = val;
	if (regIdx == REG_PMP)
		proc_load_pmp(proc);
	return proc;
}

struct proc *syscall_yield(struct proc *proc)
{
	proc->sleep = timeout_get(csrr_mhartid());
	return schedule_yield(proc);
}

struct proc *syscall_getcap(struct proc *proc, uint64_t idx)
{
	cnode_handle_t handle = cnode_get_handle(proc->pid, idx);
	proc->regs[REG_A0] = cnode_get_cap(handle).raw;
	return proc;
}

struct proc *syscall_movcap(struct proc *proc, uint64_t srcIdx, uint64_t dstIdx)
{
	// Get handles
	cnode_handle_t src_handle = cnode_get_handle(proc->pid, srcIdx);
	cnode_handle_t dst_handle = cnode_get_handle(proc->pid, dstIdx);

	if (!cnode_contains(src_handle)) {
		proc->regs[REG_A0] = EXCPT_EMPTY;
		return proc;
	}
	if (cnode_contains(dst_handle)) {
		proc->regs[REG_A0] = EXCPT_COLLISION;
		return proc;
	}
	syscall_lock();
	if (cnode_contains(src_handle)) {
		cnode_move(src_handle, dst_handle);
		proc->regs[REG_A0] = EXCPT_NONE;
	} else {
		proc->regs[REG_A0] = EXCPT_EMPTY;
	}
	syscall_unlock();
	return proc;
}

struct proc *syscall_delcap(struct proc *proc, uint64_t idx)
{
	cnode_handle_t handle = cnode_get_handle(proc->pid, idx);
	union cap cap = cnode_get_cap(handle);
	if (!cnode_contains(handle)) {
		proc->regs[REG_A0] = EXCPT_EMPTY;
		return proc;
	}
	syscall_lock();
	if (cnode_contains(handle)) {
		cnode_delete(handle);
		if (cap.type == CAPTY_TIME)
			schedule_delete(cap.time.hartid, cap.time.free, cap.time.end);
		proc->regs[REG_A0] = EXCPT_NONE;
	} else {
		proc->regs[REG_A0] = EXCPT_EMPTY;
	}
	syscall_unlock();
	return proc;
}

static void _revoke_time_hook(cnode_handle_t handle, union cap cap, union cap child_cap)
{
	cap.time.free = child_cap.time.free;
	cnode_set_cap(handle, cap);
	schedule_update(cap.time.hartid, cnode_get_pid(handle), cap.time.free, cap.time.end);
}

static void _revoke_time_post_hook(cnode_handle_t handle, union cap cap)
{
	cap.time.free = cap.time.begin;
	cnode_set_cap(handle, cap);
	schedule_update(cap.time.hartid, cnode_get_pid(handle), cap.time.free, cap.time.end);
}

static void _revoke_memory_hook(cnode_handle_t handle, union cap cap, union cap child_cap)
{
	if (cap.type == CAPTY_MEMORY) {
		cap.memory.free = child_cap.memory.free; // Inherit free region
		cap.memory.lock = child_cap.memory.lock; // Inherit lock
		cnode_set_cap(handle, cap);
	}
}

static void _revoke_memory_post_hook(cnode_handle_t handle, union cap cap)
{
	cap.memory.free = cap.memory.begin;
	cnode_set_cap(handle, cap);
}

static void _revoke_monitor_hook(cnode_handle_t handle, union cap cap, union cap child_cap)
{
	cap.monitor.free = child_cap.monitor.free;
	cnode_set_cap(handle, cap);
}

static void _revoke_monitor_post_hook(cnode_handle_t handle, union cap cap)
{
	cap.monitor.free = cap.monitor.begin;
	cnode_set_cap(handle, cap);
}

static void _revoke_channel_hook(cnode_handle_t handle, union cap cap, union cap child_cap)
{
	cap.channel.free = child_cap.channel.free;
	cnode_set_cap(handle, cap);
}

static void _revoke_channel_post_hook(cnode_handle_t handle, union cap cap)
{
	cap.channel.free = cap.channel.begin;
	cnode_set_cap(handle, cap);
}

static void _revoke_socket_hook(cnode_handle_t handle, union cap cap, union cap child_cap)
{
	/* This should be empty */
}

static void _revoke_socket_post_hook(cnode_handle_t handle, union cap cap)
{
	/* This should be empty */
}

struct proc *syscall_revcap(struct proc *proc, uint64_t idx)
{
	// Get handle
	cnode_handle_t handle = cnode_get_handle(proc->pid, idx);
	union cap cap = cnode_get_cap(handle);

	// If empty slot
	if (cnode_contains(handle)) {
		proc->regs[REG_A0] = EXCPT_EMPTY;
		return proc;
	}

	bool (*is_parent)(union cap, union cap);
	void (*hook)(cnode_handle_t, union cap, union cap);
	void (*post_hook)(cnode_handle_t, union cap);
	switch (cap.type) {
	case CAPTY_TIME:
		is_parent = cap_time_parent;
		hook = _revoke_time_hook;
		post_hook = _revoke_time_post_hook;
		break;
	case CAPTY_MEMORY:
		is_parent = cap_memory_parent;
		hook = _revoke_memory_hook;
		post_hook = _revoke_memory_post_hook;
		break;
	case CAPTY_MONITOR:
		is_parent = cap_monitor_parent;
		hook = _revoke_monitor_hook;
		post_hook = _revoke_monitor_post_hook;
		break;
	case CAPTY_CHANNEL:
		is_parent = cap_channel_parent;
		hook = _revoke_channel_hook;
		post_hook = _revoke_channel_post_hook;
		break;
	case CAPTY_SOCKET:
		is_parent = cap_socket_parent;
		hook = _revoke_socket_hook;
		post_hook = _revoke_socket_post_hook;
		break;
	default:
		proc->regs[REG_A0] = EXCPT_NONE;
		return proc;
	}

	cnode_handle_t next_handle;
	union cap next_cap;
	while (cnode_contains(handle)) {
		next_handle = cnode_get_next(handle);
		next_cap = cnode_get_cap(next_handle);
		if (!is_parent(cap, next_cap))
			break;
		syscall_lock();
		if (cnode_delete_if(next_handle, handle))
			hook(handle, cap, next_cap);
		syscall_unlock();
	}

	if (!cnode_contains(handle)) {
		proc->regs[REG_A0] = EXCPT_EMPTY;
		return proc;
	}
	syscall_lock();
	if (cnode_contains(handle)) {
		post_hook(handle, cap);
		proc->regs[REG_A0] = EXCPT_NONE;
	} else {
		proc->regs[REG_A0] = EXCPT_EMPTY;
	}
	syscall_unlock();
	return proc;
}

static void _derive_time(cnode_handle_t orig_handle, union cap orig_cap, cnode_handle_t drv_handle, union cap drv_cap)
{
	orig_cap.time.free = drv_cap.time.begin;
	cnode_set_cap(orig_handle, orig_cap);
	schedule_update(drv_cap.time.hartid, cnode_get_pid(orig_handle), drv_cap.time.begin, drv_cap.time.end);
}

static void _derive_memory(cnode_handle_t orig_handle, union cap orig_cap, cnode_handle_t drv_handle, union cap drv_cap)
{
	if (drv_cap.type == CAPTY_MEMORY) { // Memory
		orig_cap.memory.free = drv_cap.memory.begin;
	} else { // PMP
		orig_cap.memory.lock = true;
	}
	cnode_set_cap(orig_handle, orig_cap);
}

static void _derive_monitor(cnode_handle_t orig_handle, union cap orig_cap, cnode_handle_t drv_handle,
			    union cap drv_cap)
{
	orig_cap.monitor.free = drv_cap.monitor.begin;
	cnode_set_cap(orig_handle, orig_cap);
}

static void _derive_channel(cnode_handle_t orig_handle, union cap orig_cap, cnode_handle_t drv_handle,
			    union cap drv_cap)
{
	// Update free pointer.
	if (drv_cap.type == CAPTY_CHANNEL) {
		orig_cap.channel.free = drv_cap.channel.begin;
	} else {
		orig_cap.channel.free = drv_cap.socket.channel + 1;
	}
	cnode_set_cap(orig_handle, orig_cap);
}

static void _derive_socket(cnode_handle_t orig_handle, union cap orig_cap, cnode_handle_t drv_handle, union cap drv_cap)
{
	/* This should be empty */
}

struct proc *syscall_drvcap(struct proc *proc, uint64_t orig_idx, uint64_t drv_idx, union cap drv_cap)
{
	// Get handle
	cnode_handle_t orig_handle = cnode_get_handle(proc->pid, orig_idx);
	cnode_handle_t drv_handle = cnode_get_handle(proc->pid, drv_idx);
	union cap orig_cap = cnode_get_cap(orig_handle);

	if (!cnode_contains(orig_handle)) { // If empty slot
		proc->regs[REG_A0] = EXCPT_EMPTY;
		return proc;
	} else if (cnode_contains(drv_handle)) { // If destination occupied
		proc->regs[REG_A0] = EXCPT_COLLISION;
		return proc;
	}

	void (*hook)(cnode_handle_t, union cap, cnode_handle_t, union cap);
	bool (*can_derive)(union cap, union cap);
	// Call specific handler for capability or return unimplemented.
	switch (orig_cap.type) {
	case CAPTY_TIME:
		hook = _derive_time;
		can_derive = cap_time_derive;
		break;
	case CAPTY_MEMORY:
		hook = _derive_memory;
		can_derive = cap_memory_derive;
		break;
	case CAPTY_MONITOR:
		hook = _derive_monitor;
		can_derive = cap_monitor_derive;
		break;
	case CAPTY_CHANNEL:
		hook = _derive_channel;
		can_derive = cap_channel_derive;
		break;
	case CAPTY_SOCKET:
		hook = _derive_socket;
		can_derive = cap_socket_derive;
		break;
	}
	if (!can_derive(orig_cap, drv_cap)) {
		proc->regs[REG_A0] = EXCPT_DERIVATION;
	} else {
		syscall_lock();
		if (!cnode_contains(orig_handle)) {
			proc->regs[REG_A0] = EXCPT_EMPTY;
		} else {
			cnode_insert(drv_handle, drv_cap, orig_handle);
			hook(orig_handle, orig_cap, drv_handle, drv_cap);
			proc->regs[REG_A0] = EXCPT_NONE;
		}
		syscall_unlock();
	}
	return proc;
}

struct proc *syscall_recv(struct proc *proc, uint64_t recv_idx)
{
	return proc;
}

struct proc *syscall_send(struct proc *proc, uint64_t send_idx, uint64_t msg0, uint64_t msg1, uint64_t cap0,
			  uint64_t cap1, uint64_t yield)
{
	return proc;
}

struct proc *syscall_sendrecv(struct proc *proc, uint64_t send_idx, uint64_t recv_idx, uint64_t msg0, uint64_t msg1,
			      uint64_t cap0, uint64_t cap1)
{
	return proc;
}
