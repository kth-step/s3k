#pragma once

#include "cap_types.h"
#include "macro.h"
#include "proc.h"

#include <stdint.h>

typedef enum {
	// Basic Info & Registers
	SYS_GET_INFO,  // Retrieve basic system information
	SYS_REG_READ,  // Set the value of a specific register
	SYS_REG_WRITE, // Get the value of a specific register
	SYS_SYNC,      // Synchronize with capabilities/scheduling
	SYS_SLEEP,

	// Capability Management
	SYS_CAP_READ,	// Read the properties of a capability
	SYS_CAP_MOVE,	// Move a capability to a different slot
	SYS_CAP_DELETE, // Remove a capability from the system
	SYS_CAP_REVOKE, // Revoke a derived capabilities
	SYS_CAP_DERIVE, // Derive a new capability from an existing one

	// PMP
	SYS_PMP_LOAD,
	SYS_PMP_UNLOAD,

	// Monitor
	SYS_MON_SUSPEND,
	SYS_MON_RESUME,
	SYS_MON_STATE_GET,
	SYS_MON_YIELD,
	SYS_MON_REG_READ,
	SYS_MON_REG_WRITE,
	SYS_MON_CAP_READ,
	SYS_MON_CAP_MOVE,
	SYS_MON_PMP_LOAD,
	SYS_MON_PMP_UNLOAD,

	// Socket
	SYS_SOCK_SEND,
	SYS_SOCK_RECV,
	SYS_SOCK_SENDRECV,
} syscall_t;

typedef union {
	struct {
		uint64_t a0, a1, a2, a3, a4, a5, a6, a7;
	};

	struct {
		uint64_t info;
	} get_info;

	struct {
		uint64_t reg;
	} reg_read;

	struct {
		uint64_t reg;
		uint64_t val;
	} reg_write;

	struct {
		uint64_t full;
	} sync;

	struct {
		uint64_t time;
	} sleep;

	struct {
		uint64_t idx;
	} cap_read;

	struct {
		uint64_t src_idx;
		uint64_t dst_idx;
	} cap_move;

	struct {
		uint64_t idx;
	} cap_delete;

	struct {
		uint64_t idx;
	} cap_revoke;

	struct {
		uint64_t src_idx;
		uint64_t dst_idx;
		uint64_t cap_raw;
	} cap_derive;

	struct {
		uint64_t idx;
		uint64_t slot;
	} pmp_load;

	struct {
		uint64_t idx;
	} pmp_unload;

	struct {
		uint64_t mon_idx;
		uint64_t pid;
	} mon_state;

	struct {
		uint64_t mon_idx;
		uint64_t pid;
		uint64_t reg;
	} mon_reg_read;

	struct {
		uint64_t mon_idx;
		uint64_t pid;
		uint64_t reg;
		uint64_t val;
	} mon_reg_write;

	struct {
		uint64_t mon_idx;
		uint64_t pid;
		uint64_t idx;
	} mon_cap_read;

	struct {
		uint64_t mon_idx;
		uint64_t src_pid;
		uint64_t src_idx;
		uint64_t dst_pid;
		uint64_t dst_idx;
	} mon_cap_move;

	struct {
		uint64_t mon_idx;
		uint64_t pid;
		uint64_t idx;
		uint64_t slot;
	} mon_pmp_load;

	struct {
		uint64_t mon_idx;
		uint64_t pid;
		uint64_t idx;
	} mon_pmp_unload;

	struct {
		uint64_t sock_idx;
		uint64_t cap_idx;
		uint64_t send_cap;
		uint64_t data[4];
	} sock;
} sys_args_t;

_Static_assert(sizeof(sys_args_t) == 64, "sys_args_t has the wrong size");

proc_t *syscall_handler(proc_t *proc);
