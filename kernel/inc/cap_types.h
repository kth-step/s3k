#pragma once

#include "error.h"
#include "kassert.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum {
	MEM_NONE = 0,
	MEM_R = 1,
	MEM_W = 2,
	MEM_X = 4,
	MEM_RW = MEM_R | MEM_W,
	MEM_RX = MEM_R | MEM_X,
	MEM_RWX = MEM_R | MEM_W | MEM_X,
} mem_perm_t;

// IPC Modes
typedef enum {
	IPC_NOYIELD = 0, // Non-Yielding Synchronous
	IPC_YIELD = 1,	 // Yielding Synchronous
			 //	IPC_ASYNC = 2,	 // Asynchronous
} ipc_mode_t;

// IPC Permissions
typedef enum {
	IPC_SDATA = 1, // Server can send data
	IPC_SCAP = 2,  // Server can send capabilities
	IPC_CDATA = 4, // Client can send data
	IPC_CCAP = 8,  // Client can send capabilities
} ipc_perm_t;

// Capability types
typedef enum capty {
	CAPTY_NONE = 0,	   ///< No capability.
	CAPTY_TIME = 1,	   ///< Time Slice capability.
	CAPTY_MEMORY = 2,  ///< Memory Slice capability.
	CAPTY_PMP = 3,	   ///< PMP Frame capability.
	CAPTY_MONITOR = 4, ///< Monitor capability.
	CAPTY_CHANNEL = 5, ///< IPC Channel capability.
	CAPTY_SOCKET = 6,  ///< IPC Server/Client capability.
} capty_t;

/// Capability description
typedef union cap {
	uint64_t type : 4;
	uint64_t raw;

	struct {
		uint64_t type : 4;
		uint64_t unused : 4;
		uint64_t hart : 8;
		uint64_t bgn : 16;
		uint64_t mrk : 16;
		uint64_t end : 16;
	} time;

	struct {
		uint64_t type : 4;
		uint64_t rwx : 3;
		uint64_t lck : 1;
		uint64_t tag : 8;
		uint64_t bgn : 16;
		uint64_t mrk : 16;
		uint64_t end : 16;
	} mem;

	struct {
		uint64_t type : 4;
		uint64_t rwx : 3;
		uint64_t used : 1;
		uint64_t slot : 8;
		uint64_t addr : 48;
	} pmp;

	struct {
		uint64_t type : 4;
		uint64_t unused : 12;
		uint64_t bgn : 16;
		uint64_t mrk : 16;
		uint64_t end : 16;
	} mon;

	struct {
		uint64_t type : 4;
		uint64_t unused : 12;
		uint64_t bgn : 16;
		uint64_t mrk : 16;
		uint64_t end : 16;
	} chan;

	struct {
		uint64_t type : 4;
		uint64_t mode : 4;
		uint64_t perm : 8;
		uint64_t chan : 16;
		uint64_t tag : 32;
	} sock;
} cap_t;

cap_t cap_mk_time(uint64_t hart, uint64_t bgn, uint64_t end);
cap_t cap_mk_memory(uint64_t bgn, uint64_t end, uint64_t rwx);
cap_t cap_mk_pmp(uint64_t addr, uint64_t rwx);
cap_t cap_mk_monitor(uint64_t bgn, uint64_t end);
cap_t cap_mk_channel(uint64_t bgn, uint64_t end);
cap_t cap_mk_socket(uint64_t chan, uint64_t mode, uint64_t perm, uint64_t tag);

bool cap_revokable(cap_t p, cap_t c);
bool cap_derivable(cap_t p, cap_t c);
