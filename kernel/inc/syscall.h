#pragma once

#include "cap_types.h"
#include "proc.h"

#include <stdint.h>

typedef enum {
	// Basic Info & Registers
	SYSCALL_GET_INFO,  // Retrieve basic system information
	SYSCALL_REG_READ,  // Set the value of a specific register
	SYSCALL_REG_WRITE, // Get the value of a specific register
	SYSCALL_SYNC,	   // Synchronize with capabilities/scheduling
	SYSCALL_SYNC_MEM,  // Synchronize with capabilities/scheduling

	// Capability Management
	SYSCALL_CAP_READ,   // Read the properties of a capability
	SYSCALL_CAP_MOVE,   // Move a capability to a different slot
	SYSCALL_CAP_DELETE, // Remove a capability from the system
	SYSCALL_CAP_REVOKE, // Revoke a derived capabilities
	SYSCALL_CAP_DERIVE, // Derive a new capability from an existing one

	// PMP
	SYSCALL_PMP_LOAD,
	SYSCALL_PMP_UNLOAD,

	// Monitor
	SYSCALL_MONITOR_SUSPEND,
	SYSCALL_MONITOR_RESUME,
	SYSCALL_MONITOR_REG_READ,
	SYSCALL_MONITOR_REG_WRITE,
	SYSCALL_MONITOR_CAP_READ,
	SYSCALL_MONITOR_CAP_MOVE,
	SYSCALL_MONITOR_PMP_LOAD,
	SYSCALL_MONITOR_PMP_UNLOAD,

	// Socket
	SYSCALL_SOCK_SEND,
	SYSCALL_SOCK_CALL,
	SYSCALL_SOCK_REPLY,
	SYSCALL_SOCK_RECV,
	SYSCALL_SOCK_REPLYRECV,
} syscall_t;

proc_t *handle_syscall(proc_t *p);
