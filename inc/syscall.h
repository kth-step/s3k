/**
 * @file stack.h
 * @brief Definition of system calls and error codes.
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */
#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include "proc.h"

#include <stdint.h>

/// @defgroup syscall-handle System Call Handling
/// @{

/// System call exceptions
enum excpt {
	EXCPT_NONE,	    ///< No exception.
	EXCPT_EMPTY,	    ///< Capability slot is empty.
	EXCPT_COLLISION,    ///< Capability slot is occupied.
	EXCPT_DERIVATION,   ///< Capability can not be derived.
	EXCPT_PREEMPTED,    ///< System call was preempted.
	EXCPT_SUSPENDED,    ///< Process was suspended
	EXCPT_MPID,	    ///< Bad PID for monitor operation.
	EXCPT_MBUSY,	    ///< Process busy.
	EXCPT_UNIMPLEMENTED ///< System call not implemented for specified
			    ///< capability.
};

enum syscall_nr {
	// Capabilityless syscalls
	SYSCALL_GETINFO, ///< Get information about current execution.
	SYSCALL_GETREG,	 ///< Get register value
	SYSCALL_SETREG,	 ///< Set register value
	SYSCALL_YIELD,	 ///< Yield remaining time slice
	// Capability syscalls
	SYSCALL_GETCAP, ///< Get capability description
	SYSCALL_MOVCAP, ///< Move capability
	SYSCALL_DELCAP, ///< Delete capability
	SYSCALL_REVCAP, ///< Revoke capability
	SYSCALL_DRVCAP, ///< Derive capability
	// Monitor syscalls
	SYSCALL_MSUSPEND, ///< Monitor suspend process
	SYSCALL_MRESUME,  ///< Monitor resume process
	SYSCALL_MGETREG,  ///< Monitor get register value
	SYSCALL_MSETREG,  ///< Monitor set register value
	SYSCALL_MGETCAP,  ///< Monitor get capability description
	SYSCALL_MTAKECAP, ///< Monitor take capability
	SYSCALL_MGIVECAP, ///< Monitor give capability
	// IPC syscalls
	SYSCALL_RECV,	  ///< Receive message/capability
	SYSCALL_SEND,	  ///< Send message/capability
	SYSCALL_SENDRECV, ///< Send then receive message/capability
};

void syscall_lock(void);
void syscall_unlock(void);

// Simple system calls
/// Get process ID.
void syscall_getinfo(struct proc *proc, uint64_t info);
/// Get register value.
void syscall_getreg(struct proc *proc, uint64_t reg);
/// Get register value.
void syscall_setreg(struct proc *proc, uint64_t reg, uint64_t val);
/// Yield.
void syscall_yield(struct proc *proc);

// Capability system calls
/// Get capability description.
void syscall_getcap(struct proc *proc, uint64_t i);
/// Get move capability.
void syscall_movcap(struct proc *proc, uint64_t i, uint64_t j);
/// Delete capability.
void syscall_delcap(struct proc *proc, uint64_t i);
/// Revoke capability.
void syscall_revcap(struct proc *proc, uint64_t i);
/// Derive capability.
void syscall_drvcap(struct proc *proc, uint64_t i, uint64_t j, union cap cap);

// Monitor capability system calls
/// Suspend a process
void syscall_msuspend(struct proc *proc, uint64_t mon, uint64_t pid);
/// Resume a process
void syscall_mresume(struct proc *proc, uint64_t mon, uint64_t pid);
/// Get a register of a process
void syscall_mgetreg(struct proc *proc, uint64_t mon, uint64_t pid,
		     uint64_t reg);
/// Set a register of a process
void syscall_msetreg(struct proc *proc, uint64_t mon, uint64_t pid,
		     uint64_t reg, uint64_t val);
/// Read a capability of a process
void syscall_mgetcap(struct proc *proc, uint64_t mon, uint64_t pid, uint64_t i);
/// Take a capability from a process
void syscall_mtakecap(struct proc *proc, uint64_t mon, uint64_t pid, uint64_t i,
		      uint64_t j);
/// Give a capability to a process
void syscall_mgivecap(struct proc *proc, uint64_t mon, uint64_t pid, uint64_t i,
		      uint64_t j);

// IPC capability system calls
/// Receive a message.
void syscall_recv(struct proc *proc, uint64_t recv_idx);
/// Send a message.
void syscall_send(struct proc *proc, uint64_t send_idx, uint64_t msg0,
		  uint64_t msg1, uint64_t cap0, uint64_t cap1, uint64_t yield);
/// Send then receive a message.
void syscall_sendrecv(struct proc *proc, uint64_t recv_idx, uint64_t send_idx,
		      uint64_t msg0, uint64_t msg1, uint64_t cap0,
		      uint64_t cap1);
/// @}

#endif /* __SYSCALL_H__ */
