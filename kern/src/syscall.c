#include "syscall.h"

#include "csr.h"
#include "current.h"
#include "exception.h"
#include "ipc.h"
#include "lock.h"
#include "macro.h"
#include "mem.h"
#include "mon.h"
#include "preempt.h"
#include "proc.h"
#include "rtc.h"
#include "tsl.h"
#include "ttas.h"

/**
 * Get the current process's PID.
 */
static proc_t *syscall_get_pid(pid_t pid, word_t args[8])
{
	(void)args;
	args[0] = pid;
	return current;
}

/**
 * Get a virtual register of the current process.
 */
static proc_t *syscall_get_vreg(pid_t pid, word_t args[8])
{
	(void)pid;
	switch (args[1]) {
	case VREG_TPC:
		args[0] = current->trap.tpc;
		break;
	case VREG_TSP:
		args[0] = current->trap.tsp;
		break;
	case VREG_ECAUSE:
		args[0] = current->trap.ecause;
		break;
	case VREG_EVAL:
		args[0] = current->trap.eval;
		break;
	case VREG_EPC:
		args[0] = current->trap.epc;
		break;
	case VREG_ESP:
		args[0] = current->trap.esp;
		break;
	default:
		args[0] = 0;
		break;
	}
	return current;
}

/**
 * Set a virtual register of the current process.
 */
static proc_t *syscall_set_vreg(pid_t pid, word_t args[8])
{
	(void)pid;
	switch (args[1]) {
	case VREG_TPC:
		current->trap.tpc = args[2];
		break;
	case VREG_TSP:
		current->trap.tsp = args[2];
		break;
	case VREG_ECAUSE:
		current->trap.ecause = args[2];
		break;
	case VREG_EVAL:
		current->trap.eval = args[2];
		break;
	case VREG_EPC:
		current->trap.epc = args[2];
		break;
	case VREG_ESP:
		current->trap.esp = args[2];
		break;
	default:
		break;
	}
	return current;
}

/**
 * Release the process, then call the scheduler.
 */
static proc_t *syscall_sync(pid_t pid, word_t args[8])
{
	(void)pid;
	(void)args;
	current->timeout = 0;
	return NULL;
}

/**
 * Sleep until the specified timeout, then call the scheduler.
 */
static proc_t *syscall_sleep_until(pid_t pid, word_t args[8])
{
	(void)pid;
	if (args[1] != 0) {
		current->timeout = args[1];
	}
	return NULL;
}

/**
 * Get a memory capability.
 */
static proc_t *syscall_mem_get(pid_t pid, word_t args[8])
{
	args[0] = mem_get(pid, args[1], (mem_t *)&args[1]);
	return current;
}

/**
 * Get a time slice capability.
 */
static proc_t *syscall_tsl_get(pid_t pid, word_t args[8])
{
	args[0] = tsl_get(pid, args[1], (tsl_t *)&args[1]);
	return current;
}

/**
 * Get a monitor capability.
 */
static proc_t *syscall_mon_get(pid_t pid, word_t args[8])
{
	args[0] = mon_get(pid, args[1], (mon_t *)&args[1]);
	return current;
}

/**
 * Get an IPC capability.
 */
static proc_t *syscall_ipc_get(pid_t pid, word_t args[8])
{
	args[0] = ipc_get(pid, args[1], (ipc_t *)&args[1]);
	return current;
}

/**
 * Derive a memory capability.
 */
static proc_t *syscall_mem_derive(pid_t pid, word_t args[8])
{
	args[0] = mem_derive(pid, args[1], pid, args[2], args[3], args[4], args[5]);
	return current;
}

/**
 * Derive a time slice capability.
 */
static proc_t *syscall_tsl_derive(pid_t pid, word_t args[8])
{
	args[0] = tsl_derive(pid, args[1], pid, args[2], args[3], args[4]);
	return current;
}

/**
 * Derive a monitor capability.
 */
static proc_t *syscall_mon_derive(pid_t pid, word_t args[8])
{
	args[0] = mon_derive(pid, args[1], pid, args[2]);
	return current;
}

/**
 * Derive an IPC capability.
 */
static proc_t *syscall_ipc_derive(pid_t pid, word_t args[8])
{
	args[0] = ipc_derive(pid, args[1], pid, args[2], args[3], args[4]);
	return current;
}

/**
 * Revoke the children of a memory capability.
 */
static proc_t *syscall_mem_revoke(pid_t pid, word_t args[8])
{
	args[0] = mem_revoke(pid, args[1]);
	return current;
}

/**
 * Revoke the children of a time slice capability.
 */
static proc_t *syscall_tsl_revoke(pid_t pid, word_t args[8])
{
	args[0] = tsl_revoke(pid, args[1]);
	return current;
}

/**
 * Revoke the children of a monitor capability.
 */
static proc_t *syscall_mon_revoke(pid_t pid, word_t args[8])
{
	args[0] = mon_revoke(pid, args[1]);
	return current;
}

/**
 * Revoke the children of an IPC capability.
 */
static proc_t *syscall_ipc_revoke(pid_t pid, word_t args[8])
{
	args[0] = ipc_revoke(pid, args[1]);
	return current;
}

/**
 * Delete a memory capability.
 */
static proc_t *syscall_mem_delete(pid_t pid, word_t args[8])
{
	args[0] = mem_delete(pid, args[1]);
	return current;
}

/**
 * Delete a time slice capability.
 */
static proc_t *syscall_tsl_delete(pid_t pid, word_t args[8])
{
	args[0] = tsl_delete(pid, args[1]);
	return current;
}

/**
 * Delete a monitor capability.
 */
static proc_t *syscall_mon_delete(pid_t pid, word_t args[8])
{
	args[0] = mon_delete(pid, args[1]);
	return current;
}

/**
 * Delete an IPC capability
 */
static proc_t *syscall_ipc_delete(pid_t pid, word_t args[8])
{
	args[0] = ipc_delete(pid, args[1]);
	return current;
}

/**
 * Get a memory capability's PMP configuration.
 */
static proc_t *syscall_mem_pmp_get(pid_t pid, word_t args[8])
{
	pmp_slot_t slot;
	mem_perm_t rwx;
	pmp_addr_t addr;
	args[0] = mem_pmp_get(pid, args[1], &slot, &rwx, &addr);
	args[1] = slot;
	args[2] = rwx;
	args[3] = addr;
	return current;
}

/**
 * Set a memory capability's PMP configuration.
 */
static proc_t *syscall_mem_pmp_set(pid_t pid, word_t args[8])
{
	args[0] = mem_pmp_set(pid, args[1], args[2], args[3], args[4]);
	return current;
}

/**
 * Clear a memory capability's PMP configuration.
 */
static proc_t *syscall_mem_pmp_clear(pid_t pid, word_t args[8])
{
	args[0] = mem_pmp_clear(pid, args[1]);
	return current;
}

/**
 * Enable or disable a time slice capability's minor frame.
 */
static proc_t *syscall_tsl_set(pid_t pid, word_t args[8])
{
	args[0] = tsl_set(pid, args[1], args[2]);
	return current;
}

/**
 * Suspend the process that is being monitored by the specified monitor capability.
 */
static proc_t *syscall_mon_suspend(pid_t pid, word_t args[8])
{
	args[0] = mon_suspend(pid, args[1]);
	return current;
}

/**
 * Resume the process that is being monitored by the specified monitor capability.
 */
static proc_t *syscall_mon_resume(pid_t pid, word_t args[8])
{
	args[0] = mon_resume(pid, args[1]);
	return current;
}

/**
 * Yield execution time to the process being monitored by the specified monitor capability.
 */
static proc_t *syscall_mon_yield(pid_t pid, word_t args[8])
{
	proc_t *next = current;
	args[0] = mon_yield(pid, args[1], &next);
	return next;
}

/**
 * Get a register value of the process being monitored by the specified monitor capability.
 */
static proc_t *syscall_mon_reg_get(pid_t pid, word_t args[8])
{
	word_t value;
	args[0] = mon_reg_get(pid, args[1], args[2], &value);
	args[1] = value;
	return current;
}

/**
 * Set a register of the process being monitored by the specified monitor capability.
 */
static proc_t *syscall_mon_reg_set(pid_t pid, word_t args[8])
{
	args[0] = mon_reg_set(pid, args[1], args[2], args[3]);
	return current;
}

/**
 * Get a virtual register value of the process being monitored by the specified monitor capability.
 */
static proc_t *syscall_mon_vreg_get(pid_t pid, word_t args[8])
{
	word_t value;
	args[0] = mon_vreg_get(pid, args[1], args[2], &value);
	args[1] = value;
	return current;
}

/**
 * Set a virtual register of the process being monitored by the specified monitor capability.
 */
static proc_t *syscall_mon_vreg_set(pid_t pid, word_t args[8])
{
	args[0] = mon_vreg_set(pid, args[1], args[2], args[3]);
	return current;
}

/**
 * Get a time slice capability configuration of the process being monitored by the specified monitor capability.
 */
static proc_t *syscall_mon_tsl_get(pid_t pid, word_t args[8])
{
	pid_t target = mon_get_pid(pid, args[1]);
	args[0] = ERR_INVALID_ACCESS;
	if (target != INVALID_PID) {
		args[0] = tsl_get(target, args[2], (tsl_t *)&args[1]);
	}
	return current;
}

/**
 * Get a memory capability configuration of the process being monitored by the specified monitor capability.
 */
static proc_t *syscall_mon_mem_get(pid_t pid, word_t args[8])
{
	pid_t target = mon_get_pid(pid, args[1]);
	args[0] = ERR_INVALID_ACCESS;
	if (target != INVALID_PID) {
		args[0] = mem_get(target, args[2], (mem_t *)&args[1]);
	}
	return current;
}

/**
 * Get a monitor capability configuration of the process being monitored by the specified monitor capability.
 */
static proc_t *syscall_mon_mon_get(pid_t pid, word_t args[8])
{
	pid_t target = mon_get_pid(pid, args[1]);
	args[0] = ERR_INVALID_ACCESS;
	if (target != INVALID_PID) {
		args[0] = mon_get(target, args[2], (mon_t *)&args[1]);
	}
	return current;
}

/**
 * Get an IPC capability configuration of the process being monitored by the specified monitor capability.
 */
static proc_t *syscall_mon_ipc_get(pid_t pid, word_t args[8])
{
	pid_t target = mon_get_pid(pid, args[1]);
	args[0] = ERR_INVALID_ACCESS;
	if (target != INVALID_PID) {
		args[0] = ipc_get(target, args[2], (ipc_t *)&args[1]);
	}
	return current;
}

/**
 * Grant a memory capability to the process being monitored by the specified monitor capability.
 */
static proc_t *syscall_mon_mem_grant(pid_t pid, word_t args[8])
{
	pid_t target = mon_get_pid(pid, args[1]);
	args[0] = ERR_INVALID_ACCESS;
	if (target != INVALID_PID) {
		args[0] = mem_transfer(pid, args[2], target);
	}
	return current;
}

/**
 * Grant a time slice capability to the process being monitored by the specified monitor capability.
 */
static proc_t *syscall_mon_tsl_grant(pid_t pid, word_t args[8])
{
	pid_t target = mon_get_pid(pid, args[1]);
	args[0] = ERR_INVALID_ACCESS;
	if (target != INVALID_PID) {
		args[0] = tsl_transfer(pid, args[2], target);
	}
	return current;
}

/**
 * Grant a monitor capability to the process being monitored by the specified monitor capability.
 */
static proc_t *syscall_mon_mon_grant(pid_t pid, word_t args[8])
{
	pid_t target = mon_get_pid(pid, args[1]);
	args[0] = ERR_INVALID_ACCESS;
	if (target != INVALID_PID) {
		args[0] = mon_transfer(pid, args[2], target);
	}
	return current;
}

/**
 * Grant an IPC capability to the process being monitored by the specified monitor capability.
 */
static proc_t *syscall_mon_ipc_grant(pid_t pid, word_t args[8])
{
	pid_t target = mon_get_pid(pid, args[1]);
	args[0] = ERR_INVALID_ACCESS;
	if (target != INVALID_PID) {
		args[0] = ipc_transfer(pid, args[2], target);
	}
	return current;
}

/**
 * Derive then grant a time slice capability to the process being monitored by the specified monitor capability.
 */
static proc_t *syscall_mon_tsl_derive(pid_t pid, word_t args[8])
{
	pid_t target = mon_get_pid(pid, args[1]);
	args[0] = ERR_INVALID_ACCESS;
	if (target != INVALID_PID) {
		args[0] = tsl_derive(pid, args[2], target, args[3], args[4], args[5]);
	}
	return current;
}

/**
 * Get a memory capability configuration of the process being monitored by the specified monitor capability.
 */
static proc_t *syscall_mon_mem_derive(pid_t pid, word_t args[8])
{
	pid_t target = mon_get_pid(pid, args[1]);
	args[0] = ERR_INVALID_ACCESS;
	if (target != INVALID_PID) {
		args[0] = mem_derive(pid, args[2], target, args[3], args[4], args[5], args[6]);
	}
	return current;
}

/**
 * Get a monitor capability configuration of the process being monitored by the specified monitor capability.
 */
static proc_t *syscall_mon_mon_derive(pid_t pid, word_t args[8])
{
	pid_t target = mon_get_pid(pid, args[1]);
	args[0] = ERR_INVALID_ACCESS;
	if (target != INVALID_PID) {
		args[0] = mon_derive(pid, args[2], target, args[3]);
	}
	return current;
}

/**
 * Get an IPC capability configuration of the process being monitored by the specified monitor capability.
 */
static proc_t *syscall_mon_ipc_derive(pid_t pid, word_t args[8])
{
	pid_t target = mon_get_pid(pid, args[1]);
	args[0] = ERR_INVALID_ACCESS;
	if (target != INVALID_PID) {
		args[0] = ipc_derive(pid, args[2], target, args[3], args[4], args[5]);
	}
	return current;
}

/**
 * Get a memory capability PMP configuration of the process being monitored by the specified monitor capability.
 */
static proc_t *syscall_mon_mem_pmp_get(pid_t pid, word_t args[8])
{
	pid_t target = mon_get_pid(pid, args[1]);
	args[0] = ERR_INVALID_ACCESS;
	if (target != INVALID_PID) {
		pmp_slot_t slot;
		mem_perm_t rwx;
		pmp_addr_t addr;
		args[0] = mem_pmp_get(target, args[2], &slot, &rwx, &addr);
		args[1] = slot;
		args[2] = rwx;
		args[3] = addr;
	}
	return current;
}

/**
 * Set a memory capability PMP configuration of the process being monitored by the specified monitor capability.
 */
static proc_t *syscall_mon_mem_pmp_set(pid_t pid, word_t args[8])
{
	pid_t target = mon_get_pid(pid, args[1]);
	args[0] = ERR_INVALID_ACCESS;
	if (target != INVALID_PID) {
		args[0] = mem_pmp_set(target, args[2], args[3], args[4], args[5]);
	}
	return current;
}

/**
 * Clear a memory capability PMP configuration of the process being monitored by the specified monitor capability.
 */
static proc_t *syscall_mon_mem_pmp_clear(pid_t pid, word_t args[8])
{
	pid_t target = mon_get_pid(pid, args[1]);
	args[0] = ERR_INVALID_ACCESS;
	if (target != INVALID_PID) {
		args[0] = mem_pmp_clear(target, args[2]);
	}
	return current;
}

/**
 * Grant a time slice capability to the process being monitored by the specified monitor capability.
 */
static proc_t *syscall_mon_tsl_set(pid_t pid, word_t args[8])
{
	pid_t target = mon_get_pid(pid, args[1]);
	args[0] = ERR_INVALID_ACCESS;
	if (target != INVALID_PID) {
		args[0] = tsl_set(target, args[2], args[3]);
	}
	return current;
}

/**
 * Send an synchronous IPC message in a unidirectional IPC channel.
 */
static proc_t *syscall_ipc_send(pid_t pid, word_t args[8])
{
	proc_t *next = current;
	word_t data[2] = {args[2], args[3]};
	args[0] = ipc_send(pid, args[1], data, args[4], args[5], &next);
	return next;
}

/**
 * Wait to receive an IPC message (synchronous, bidirectional/unidirectional IPC).
 */
static proc_t *syscall_ipc_recv(pid_t pid, word_t args[8])
{
	proc_t *next = current;
	args[0] = ipc_recv(pid, args[1], &next);
	return next;
}

/**
 * Make an IPC call (synchronous, bidirectional IPC channel) to an IPC server.
 */
static proc_t *syscall_ipc_call(pid_t pid, word_t args[8])
{
	proc_t *next = current;
	word_t data[2] = {args[2], args[3]};
	args[0] = ipc_call(pid, args[1], data, args[4], args[5], &next);
	return next;
}

/**
 * Send a reply to an IPC call.
 */
static proc_t *syscall_ipc_reply(pid_t pid, word_t args[8])
{
	proc_t *next = current;
	word_t data[2] = {args[2], args[3]};
	args[0] = ipc_reply(pid, args[1], data, args[4], args[5], &next);
	return next;
}

/**
 * Send a reply for an IPC call, then atomically wait to receive an IPC message.
 */
static proc_t *syscall_ipc_replyrecv(pid_t pid, word_t args[8])
{
	proc_t *next = current;
	word_t data[2] = {args[2], args[3]};
	args[0] = ipc_replyrecv(pid, args[1], data, args[4], args[5], &next);
	return next;
}

/**
 * Send an asynchronous IPC message in a unidirectional IPC channel.
 */
static proc_t *syscall_ipc_asend(pid_t pid, word_t args[8])
{
	proc_t *next = current;
	args[0] = ipc_asend(pid, args[1], args[2], &next);
	return next;
}

/**
 * Read the message inbox of an asynchronous IPC channel.
 */
static proc_t *syscall_ipc_arecv(pid_t pid, word_t args[8])
{
	proc_t *next = current;
	word_t data;
	args[0] = ipc_arecv(pid, args[1], &data);
	args[1] = data;
	return next;
}

/**
 * Handler type for system calls.
 */
typedef proc_t *(*handler_t)(pid_t pid, word_t args[8]);

/**
 * Handlers for individual system calls.
 */
handler_t handlers[] = {
	syscall_get_pid,	 syscall_get_vreg,	  syscall_set_vreg,	     syscall_sync,
	syscall_sleep_until,	 syscall_mem_get,	  syscall_tsl_get,	     syscall_mon_get,
	syscall_ipc_get,	 syscall_mem_derive,	  syscall_tsl_derive,	     syscall_mon_derive,
	syscall_ipc_derive,	 syscall_mem_revoke,	  syscall_tsl_revoke,	     syscall_mon_revoke,
	syscall_ipc_revoke,	 syscall_mem_delete,	  syscall_tsl_delete,	     syscall_mon_delete,
	syscall_ipc_delete,	 syscall_mem_pmp_get,	  syscall_mem_pmp_set,	     syscall_mem_pmp_clear,
	syscall_tsl_set,	 syscall_mon_suspend,	  syscall_mon_resume,	     syscall_mon_yield,
	syscall_mon_vreg_get,	 syscall_mon_vreg_set,	  syscall_mon_reg_get,	     syscall_mon_reg_set,
	syscall_mon_mem_get,	 syscall_mon_tsl_get,	  syscall_mon_mon_get,	     syscall_mon_ipc_get,
	syscall_mon_mem_grant,	 syscall_mon_tsl_grant,	  syscall_mon_mon_grant,     syscall_mon_ipc_grant,
	syscall_mon_mem_derive,	 syscall_mon_tsl_derive,  syscall_mon_mon_derive,    syscall_mon_ipc_derive,
	syscall_mon_mem_pmp_get, syscall_mon_mem_pmp_set, syscall_mon_mem_pmp_clear, syscall_mon_tsl_set,
	syscall_ipc_send,	 syscall_ipc_recv,	  syscall_ipc_call,	     syscall_ipc_reply,
	syscall_ipc_replyrecv,	 syscall_ipc_asend,	  syscall_ipc_arecv,
};

/**
 * System call handler.
 */
proc_t *syscall_handler(void)
{
	// The system call number.
	word_t syscall_nr = current->regs.a0;

	// If system call number is invalid, make an exception.
	if (syscall_nr >= ARRAY_SIZE(handlers)) {
		return exception_handler(0x8, syscall_nr);
	}

	// Try to acquire a lock. Also checks for preemption.
	if (!lock_acquire(true)) {
		return NULL;
	}

	// Advance the program counter.
	current->regs.pc += 4;

	// Call the system call handler
	proc_t *next = handlers[syscall_nr](current->pid, &current->regs.a0);

	// Releases the lock.
	lock_release();

	return next;
}
