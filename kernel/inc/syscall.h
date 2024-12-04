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
		val_t a0, a1, a2, a3, a4, a5, a6, a7;
	};

	struct {
		val_t info;
	} get_info;

	struct {
		val_t reg;
	} reg_read;

	struct {
		val_t reg;
		val_t val;
	} reg_write;

	struct {
		val_t full;
	} sync;

	struct {
		val_t time;
	} sleep;

	struct {
		val_t idx;
	} cap_read;

	struct {
		val_t src_idx;
		val_t dst_idx;
	} cap_move;

	struct {
		val_t idx;
	} cap_delete;

	struct {
		val_t idx;
	} cap_revoke;

	struct {
		val_t src_idx;
		val_t dst_idx;
		uint64_t cap_raw;
	} cap_derive;

	struct {
		val_t idx;
		val_t slot;
	} pmp_load;

	struct {
		val_t idx;
	} pmp_unload;

	struct {
		val_t mon_idx;
		val_t pid;
	} mon_state;

	struct {
		val_t mon_idx;
		val_t pid;
		val_t reg;
	} mon_reg_read;

	struct {
		val_t mon_idx;
		val_t pid;
		val_t reg;
		val_t val;
	} mon_reg_write;

	struct {
		val_t mon_idx;
		val_t pid;
		val_t idx;
	} mon_cap_read;

	struct {
		val_t mon_idx;
		val_t src_pid;
		val_t src_idx;
		val_t dst_pid;
		val_t dst_idx;
	} mon_cap_move;

	struct {
		val_t mon_idx;
		val_t pid;
		val_t idx;
		val_t slot;
	} mon_pmp_load;

	struct {
		val_t mon_idx;
		val_t pid;
		val_t idx;
	} mon_pmp_unload;

	struct {
		val_t sock_idx;
		val_t cap_idx;
		val_t send_cap;
		val_t data[4];
	} sock;
} sys_args_t;

_Static_assert(sizeof(sys_args_t) == 32, "sys_args_t has the wrong size");

proc_t *syscall_handler(proc_t *proc);
