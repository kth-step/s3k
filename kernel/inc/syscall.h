#pragma once

#include "cap_types.h"
#include "proc.h"

#include <stdint.h>

typedef enum {
	// Basic Info & Registers
	SYS_GET_INFO,  // Retrieve basic system information
	SYS_REG_READ,  // Set the value of a specific register
	SYS_REG_WRITE, // Get the value of a specific register
	SYS_SYNC,      // Synchronize with capabilities/scheduling

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
	SYS_MON_YIELD,
	SYS_MON_REG_READ,
	SYS_MON_REG_WRITE,
	SYS_MON_CAP_READ,
	SYS_MON_CAP_MOVE,
	SYS_MON_PMP_LOAD,
	SYS_MON_PMP_UNLOAD,

	// Socket
	SYS_SOCK_SEND,
	SYS_SOCK_SENDRECV,
} syscall_t;

typedef union {
	struct {
		uint64_t a0, a1, a2, a3, a4, a5, a6, a7;
	};

	struct {
		int info;
	} get_info;

	struct {
		regnr_t reg;
		uint64_t val;
	} reg;

	struct {
		bool full;
	} sync;

	struct {
		cidx_t idx;
		cidx_t dst_idx;
		cap_t cap;
	} cap;

	struct {
		cidx_t pmp_idx;
		pmp_slot_t pmp_slot;
	} pmp;

	struct {
		cidx_t mon_idx;
		pid_t pid;
	} mon_state;

	struct {
		cidx_t mon_idx;
		pid_t pid;
		regnr_t reg;
		uint64_t val;
	} mon_reg;

	struct {
		cidx_t mon_idx;
		pid_t pid;
		cidx_t idx;
		pid_t dst_pid;
		cidx_t dst_idx;
	} mon_cap;

	struct {
		cidx_t mon_idx;
		pid_t pid;
		cidx_t pmp_idx;
		pmp_slot_t pmp_slot;
	} mon_pmp;

	struct {
		cidx_t sock_idx;
		cidx_t cbuf_idx;
		bool send_cap;
		uint64_t data[4];
		uint64_t serv_time;
	} sock;
} sys_args_t;

_Static_assert(sizeof(sys_args_t) == 64, "sys_args_t has the wrong size");

void handle_syscall(proc_t *p) __attribute__((noreturn));
