/**
 * @file stack.h
 * @brief Definition of system calls and error codes.
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */
#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include <stdint.h>

#include "proc.h"

/// @defgroup syscall-handle System Call Handling
/// @{

/// System call exceptions
typedef enum excpt {
	EXCPT_NONE,	     ///< No exception.
	EXCPT_EMPTY,	     ///< Capability slot is empty.
	EXCPT_COLLISION,     ///< Capability slot is occupied.
	EXCPT_DERIVATION,    ///< Capability can not be derived.
	EXCPT_PREEMPTED,     ///< System call was preempted.
	EXCPT_SUSPENDED,     ///< Process was suspended
	EXCPT_MPID,	     ///< Bad PID for monitor operation.
	EXCPT_MBUSY,	     ///< Process busy.
	EXCPT_UNIMPLEMENTED  ///< System call not implemented for specified capability.
} Excpt;

// Simple system calls
/// Get process ID.
uint64_t syscall_getpid(void);
/// Get register value.
uint64_t syscall_getreg(uint64_t reg);
/// Get register value.
uint64_t syscall_setreg(uint64_t reg, uint64_t val);
/// Yield.
uint64_t syscall_yield(void);

// Capability system calls
/// Get capability description.
uint64_t syscall_getcap(uint64_t i);
/// Get move capability.
uint64_t syscall_movcap(uint64_t i, uint64_t j);
/// Delete capability.
uint64_t syscall_delcap(uint64_t i);
/// Revoke capability.
uint64_t syscall_revcap(uint64_t i);
/// Derive capability.
uint64_t syscall_drvcap(uint64_t i, uint64_t j, union cap cap);

// Monitor capability system calls
/// Suspend a process
uint64_t syscall_msuspend(uint64_t mon, uint64_t pid);
/// Resume a process
uint64_t syscall_mresume(uint64_t mon, uint64_t pid);
/// Get a register of a process
uint64_t syscall_mgetreg(uint64_t mon, uint64_t pid, uint64_t reg);
/// Set a register of a process
uint64_t syscall_msetreg(uint64_t mon, uint64_t pid, uint64_t reg, uint64_t val);
/// Read a capability of a process
uint64_t syscall_mgetcap(uint64_t mon, uint64_t pid, uint64_t i);
/// Take a capability from a process
uint64_t syscall_mtakecap(uint64_t mon, uint64_t pid, uint64_t i, uint64_t j);
/// Give a capability to a process
uint64_t syscall_mgivecap(uint64_t mon, uint64_t pid, uint64_t i, uint64_t j);

// IPC capability system calls
/// Receive a message.
uint64_t syscall_recv(uint64_t recv_idx);
/// Send a message.
uint64_t syscall_send(uint64_t send_idx, uint64_t msg0, uint64_t msg1, uint64_t cap0, uint64_t cap1,
		      uint64_t yield);
/// Send then receive a message.
uint64_t syscall_sendrecv(uint64_t recv_idx, uint64_t send_idx, uint64_t msg0, uint64_t msg1,
			  uint64_t cap0, uint64_t cap1);
/// @}

#endif /* __SYSCALL_H__ */
