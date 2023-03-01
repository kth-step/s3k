/* See LICENSE file for copyright and license details. */
#include "syscall.h"

#include "bsp/timer.h"
#include "cap.h"
#include "common.h"
#include "consts.h"
#include "csr.h"
#include "sched.h"
#include "trap.h"

// Lock used for capability operations.
static struct ticket_lock cap_lock;

// static struct proc *listeners[NCHANNEL];

static void _lock(void)
{
	tl_lock(&cap_lock);
}

static void _unlock(void)
{
	tl_unlock(&cap_lock);
}

/*** System call handlers ***/
uint64_t syscall_getpid()
{
	return current->pid;
}

uint64_t syscall_getreg(uint64_t regIdx)
{
	uint64_t *regs = (uint64_t *)&current->regs;
	return regs[regIdx % REG_COUNT];
}

uint64_t syscall_setreg(uint64_t regIdx, uint64_t val)
{
	uint64_t *regs = (uint64_t *)&current->regs;
	uint64_t tmp = regs[regIdx % REG_COUNT];
	regs[regIdx % REG_COUNT] = val;
	if (regIdx == REG_PMP)
		proc_load_pmp(current);
	return tmp;
}

uint64_t syscall_yield(void)
{
	current->sleep = timeout_get(csrr_mhartid());
	sched_yield();
	return current->regs[REG_A0];
}

uint64_t syscall_getcap(uint64_t idx)
{
	cnode_handle_t handle = cnode_get_handle(current->pid, idx);
	return cnode_get_cap(handle).raw;
}

uint64_t syscall_movcap(uint64_t srcIdx, uint64_t dstIdx)
{
	// Get handles
	cnode_handle_t src_handle = cnode_get_handle(current->pid, srcIdx);
	cnode_handle_t dst_handle = cnode_get_handle(current->pid, dstIdx);

	if (!cnode_contains(src_handle)) {
		return EXCPT_EMPTY;
	}
	if (cnode_contains(dst_handle)) {
		return EXCPT_COLLISION;
	}
	_lock();
	if (!cnode_contains(src_handle)) {
		_unlock();
		return EXCPT_EMPTY;
	}
	cnode_move(src_handle, dst_handle);
	_unlock();
	return EXCPT_NONE;
}

uint64_t syscall_delcap(uint64_t idx)
{
	cnode_handle_t handle = cnode_get_handle(current->pid, idx);
	union cap cap = cnode_get_cap(handle);
	if (!cnode_contains(handle))
		return EXCPT_EMPTY;

	_lock();
	if (!cnode_contains(handle)) {
		_unlock();
		return EXCPT_EMPTY;
	}

	cnode_delete(handle);
	if (cap.type == CAPTY_TIME)
		sched_delete(cap.time.hartid, cap.time.free, cap.time.end);

	_unlock();
	return EXCPT_NONE;
}

static void _revoke_time_hook(cnode_handle_t handle, union cap cap, union cap child_cap)
{
	cap.time.free = child_cap.time.free;
	cnode_set_cap(handle, cap);
	sched_update(cap.time.hartid, current->pid, cap.time.free, cap.time.end);
}

static void _revoke_time_post_hook(cnode_handle_t handle, union cap cap)
{
	cap.time.free = cap.time.begin;
	cnode_set_cap(handle, cap);
	sched_update(cap.time.hartid, current->pid, cap.time.free, cap.time.end);
}

static void _revoke_memory_hook(cnode_handle_t handle, union cap cap, union cap child_cap)
{
	if (cap.type == CAPTY_MEMORY) {
		cap.memory.free = child_cap.memory.free;  // Inherit free region
		cap.memory.lock = child_cap.memory.lock;  // Inherit lock
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
}

static void _revoke_socket_post_hook(cnode_handle_t handle, union cap cap)
{
}

uint64_t syscall_revcap(uint64_t idx)
{
	// Get handle
	cnode_handle_t handle = cnode_get_handle(current->pid, idx);
	union cap cap = cnode_get_cap(handle);

	// If empty slot
	if (!cnode_contains(handle))
		return EXCPT_EMPTY;

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
		return EXCPT_NONE;
	}

	cnode_handle_t next_handle;
	union cap next_cap;
	while (cnode_contains(handle)) {
		next_handle = cnode_get_next(handle);
		next_cap = cnode_get_cap(next_handle);
		if (!is_parent(cap, next_cap))
			break;
		_lock();
		if (cnode_delete_if(next_handle, handle))
			hook(handle, cap, next_cap);
		_unlock();
	}
	if (!cnode_contains(handle))
		return EXCPT_EMPTY;
	_lock();
	if (!cnode_contains(handle)) {
		_unlock();
		return EXCPT_EMPTY;
	}
	post_hook(handle, cap);
	_unlock();
	return EXCPT_NONE;
}

static void _derive_time(cnode_handle_t orig_handle, union cap orig_cap, cnode_handle_t drv_handle,
			 union cap drv_cap)
{
	orig_cap.time.free = drv_cap.time.begin;
	cnode_set_cap(orig_handle, orig_cap);
	sched_update(drv_cap.time.hartid, current->pid, drv_cap.time.begin, drv_cap.time.end);
}

static void _derive_memory(cnode_handle_t orig_handle, union cap orig_cap,
			   cnode_handle_t drv_handle, union cap drv_cap)
{
	if (drv_cap.type == CAPTY_MEMORY) {  // Memory
		orig_cap.memory.free = drv_cap.memory.begin;
	} else {  // PMP
		orig_cap.memory.lock = true;
	}
	cnode_set_cap(orig_handle, orig_cap);
}

static void _derive_monitor(cnode_handle_t orig_handle, union cap orig_cap,
			    cnode_handle_t drv_handle, union cap drv_cap)
{
	orig_cap.monitor.free = drv_cap.monitor.begin;
	cnode_set_cap(orig_handle, orig_cap);
}

static void _derive_channel(cnode_handle_t orig_handle, union cap orig_cap,
			    cnode_handle_t drv_handle, union cap drv_cap)
{
	// Update free pointer.
	if (drv_cap.type == CAPTY_CHANNEL) {
		orig_cap.channel.free = drv_cap.channel.begin;
	} else {
		orig_cap.channel.free = drv_cap.socket.channel + 1;
	}
	cnode_set_cap(orig_handle, orig_cap);
}

static void _derive_socket(cnode_handle_t orig_handle, union cap orig_cap,
			   cnode_handle_t drv_handle, union cap drv_cap)
{
}

uint64_t syscall_drvcap(uint64_t orig_idx, uint64_t drv_idx, union cap drv_cap)
{
	// Get handle
	cnode_handle_t orig_handle = cnode_get_handle(current->pid, orig_idx);
	cnode_handle_t drv_handle = cnode_get_handle(current->pid, drv_idx);
	union cap orig_cap = cnode_get_cap(orig_handle);

	// If empty slot
	if (!cnode_contains(orig_handle))
		return EXCPT_EMPTY;

	// If destination occupied
	if (cnode_contains(drv_handle))
		return EXCPT_COLLISION;

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
	if (!can_derive(orig_cap, drv_cap))
		return EXCPT_DERIVATION;
	_lock();
	if (!cnode_contains(orig_handle)) {
		_unlock();
		return EXCPT_EMPTY;
	}
	cnode_insert(drv_handle, drv_cap, orig_handle);
	hook(orig_handle, orig_cap, drv_handle, drv_cap);
	_unlock();
	return EXCPT_NONE;
}

static uint64_t check_pid(union cap cap, uint64_t pid)
{
	if (cap.type != CAPTY_MONITOR)
		return EXCPT_UNIMPLEMENTED;
	if (cap.monitor.free > pid || pid > cap.monitor.end)
		return EXCPT_MPID;
	return EXCPT_NONE;
}

/// Suspends process pid
uint64_t syscall_msuspend(uint64_t mon_idx, uint64_t pid)
{
	cnode_handle_t mon_handle = cnode_get_handle(current->pid, mon_idx);
	union cap cap = cnode_get_cap(mon_handle);
	if (!cnode_contains(mon_handle))
		return EXCPT_EMPTY;
	if (check_pid(cap, pid) != EXCPT_NONE)
		return check_pid(cap, pid);
	_lock();
	if (!cnode_contains(mon_handle)) {
		_unlock();
		return EXCPT_EMPTY;
	}
	__sync_fetch_and_or(&processes[pid].state, PS_SUSPENDED);
	_unlock();
	return EXCPT_NONE;
}

/// Resumes process pid
uint64_t syscall_mresume(uint64_t mon_idx, uint64_t pid)
{
	cnode_handle_t mon_handle = cnode_get_handle(current->pid, mon_idx);
	union cap cap = cnode_get_cap(mon_handle);
	if (!cnode_contains(mon_handle))
		return EXCPT_EMPTY;
	if (check_pid(cap, pid) != EXCPT_NONE)
		return check_pid(cap, pid);
	_lock();
	if (!cnode_contains(mon_handle)) {
		_unlock();
		return EXCPT_EMPTY;
	}
	__sync_fetch_and_and(&processes[pid].state, ~PSF_SUSPEND);
	_unlock();
	return EXCPT_NONE;
}

uint64_t syscall_mgetreg(uint64_t mon_idx, uint64_t pid, uint64_t reg)
{
	cnode_handle_t mon_handle = cnode_get_handle(current->pid, mon_idx);
	union cap cap = cnode_get_cap(mon_handle);
	if (!cnode_contains(mon_handle))
		return EXCPT_EMPTY;
	if (check_pid(cap, pid) != EXCPT_NONE)
		return check_pid(cap, pid);
	_lock();
	if (!cnode_contains(mon_handle)) {
		_unlock();
		return EXCPT_EMPTY;
	}
	struct proc *proc = &processes[pid];
	if (!__sync_bool_compare_and_swap(&proc->state, PS_SUSPENDED, PS_SUSPENDED_BUSY)) {
		_unlock();
		return EXCPT_MBUSY;
	}
	uint64_t *regs = (uint64_t *)&proc->regs;
	current->regs[REG_A1] = regs[reg % REG_COUNT];
	__sync_fetch_and_and(&proc->state, ~PSF_BUSY);
	_unlock();
	return EXCPT_NONE;
}

uint64_t syscall_msetreg(uint64_t mon_idx, uint64_t pid, uint64_t reg, uint64_t val)
{
	cnode_handle_t mon_handle = cnode_get_handle(current->pid, mon_idx);
	union cap cap = cnode_get_cap(mon_handle);
	if (!cnode_contains(mon_handle))
		return EXCPT_EMPTY;
	if (check_pid(cap, pid) != EXCPT_NONE)
		return check_pid(cap, pid);
	_lock();
	if (!cnode_contains(mon_handle)) {
		_unlock();
		return EXCPT_EMPTY;
	}
	struct proc *proc = &processes[pid];
	if (!__sync_bool_compare_and_swap(&proc->state, PS_SUSPENDED, PS_SUSPENDED_BUSY)) {
		_unlock();
		return EXCPT_MBUSY;
	}
	uint64_t *regs = (uint64_t *)&proc->regs;
	regs[reg % REG_COUNT] = val;
	__sync_fetch_and_and(&proc->state, ~PSF_BUSY);
	_unlock();
	return EXCPT_NONE;
}

uint64_t syscall_mgetcap(uint64_t mon_idx, uint64_t pid, uint64_t node_idx)
{
	cnode_handle_t mon_handle = cnode_get_handle(current->pid, mon_idx);
	union cap cap = cnode_get_cap(mon_handle);
	if (!cnode_contains(mon_handle))
		return EXCPT_EMPTY;
	if (check_pid(cap, pid) != EXCPT_NONE)
		return check_pid(cap, pid);
	_lock();
	if (!cnode_contains(mon_handle)) {
		_unlock();
		return EXCPT_EMPTY;
	}
	struct proc *proc = &processes[pid];
	if (!__sync_bool_compare_and_swap(&proc->state, PS_SUSPENDED, PS_SUSPENDED_BUSY)) {
		_unlock();
		return EXCPT_MBUSY;
	}
	cnode_handle_t node_handle = cnode_get_handle(pid, node_idx);
	union cap node_cap = cnode_get_cap(node_handle);
	current->regs[REG_A1] = node_cap.raw;
	__sync_fetch_and_and(&proc->state, ~PSF_BUSY);
	_unlock();
	return EXCPT_NONE;
}

uint64_t syscall_mtakecap(uint64_t mon_idx, uint64_t pid, uint64_t src_idx, uint64_t dst_idx)
{
	cnode_handle_t mon_handle = cnode_get_handle(current->pid, mon_idx);
	union cap cap = cnode_get_cap(mon_handle);
	if (!cnode_contains(mon_handle))
		return EXCPT_EMPTY;
	if (check_pid(cap, pid) != EXCPT_NONE)
		return check_pid(cap, pid);
	_lock();
	if (!cnode_contains(mon_handle)) {
		_unlock();
		return EXCPT_EMPTY;
	}
	struct proc *proc = &processes[pid];
	if (!__sync_bool_compare_and_swap(&proc->state, PS_SUSPENDED, PS_SUSPENDED_BUSY)) {
		_unlock();
		return EXCPT_MBUSY;
	}
	cnode_handle_t src_handle = cnode_get_handle(pid, src_idx);
	cnode_handle_t dst_handle = cnode_get_handle(current->pid, dst_idx);
	if (cnode_contains(src_handle) && !cnode_contains(dst_handle)) {
		union cap src_cap = cnode_get_cap(src_handle);
		cnode_move(src_handle, dst_handle);
		if (src_cap.type == CAPTY_TIME) {
			sched_update(src_cap.time.hartid, current->pid, src_cap.time.free,
				     src_cap.time.end);
		}
	}
	__sync_fetch_and_and(&proc->state, ~PSF_BUSY);
	_unlock();
	return EXCPT_NONE;
}

uint64_t syscall_mgivecap(uint64_t mon_idx, uint64_t pid, uint64_t src_idx, uint64_t dst_idx)
{
	cnode_handle_t mon_handle = cnode_get_handle(current->pid, mon_idx);
	union cap cap = cnode_get_cap(mon_handle);
	if (!cnode_contains(mon_handle))
		return EXCPT_EMPTY;
	if (check_pid(cap, pid) != EXCPT_NONE)
		return check_pid(cap, pid);
	_lock();
	if (!cnode_contains(mon_handle)) {
		_unlock();
		return EXCPT_EMPTY;
	}
	struct proc *proc = &processes[pid];
	if (!__sync_bool_compare_and_swap(&proc->state, PS_SUSPENDED, PS_SUSPENDED_BUSY)) {
		_unlock();
		return EXCPT_MBUSY;
	}
	cnode_handle_t src_handle = cnode_get_handle(current->pid, src_idx);
	cnode_handle_t dst_handle = cnode_get_handle(pid, dst_idx);
	if (cnode_contains(src_handle) && !cnode_contains(dst_handle)) {
		union cap src_cap = cnode_get_cap(src_handle);
		cnode_move(src_handle, dst_handle);
		if (src_cap.type == CAPTY_TIME) {
			sched_update(src_cap.time.hartid, pid, src_cap.time.free, src_cap.time.end);
		}
	}
	__sync_fetch_and_and(&proc->state, ~PSF_BUSY);
	_unlock();
	return EXCPT_NONE;
}

uint64_t syscall_recv(uint64_t recv_idx)
{
	return EXCPT_UNIMPLEMENTED;
}

uint64_t syscall_send(uint64_t send_idx, uint64_t msg0, uint64_t msg1, uint64_t cap0, uint64_t cap1,
		      uint64_t yield)
{
	return EXCPT_UNIMPLEMENTED;
}

uint64_t syscall_sendrecv(uint64_t send_idx, uint64_t recv_idx, uint64_t msg0, uint64_t msg1,
			  uint64_t cap0, uint64_t cap1)
{
	return EXCPT_UNIMPLEMENTED;
}

