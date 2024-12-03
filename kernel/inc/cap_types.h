#pragma once

#include <stdbool.h>
#include <stdint.h>

// Min logarithmic size of a memory slice
#define MIN_BLOCK_SIZE 12

// Max logarithmic size of a memory slice
#define MAX_BLOCK_SIZE 27

typedef uint32_t napot_t;
typedef uint32_t addr_t;
typedef uint32_t val_t;
typedef uint32_t time_t;
typedef uint16_t block_t;
typedef uint16_t chan_t;
typedef uint16_t time_slot_t;
typedef uint16_t pid_t;
typedef uint16_t cidx_t;
typedef uint8_t hart_t;
typedef uint8_t tag_t;
typedef uint8_t rwx_t;
typedef uint8_t pmp_slot_t;
/* register number */
typedef int regnr_t;

typedef enum {
	MEM_NONE = 0,
	MEM_R = 0x1,
	MEM_W = 0x2,
	MEM_X = 0x4,
	MEM_RW = MEM_R | MEM_W,
	MEM_RX = MEM_R | MEM_X,
	MEM_RWX = MEM_R | MEM_W | MEM_X,
} mem_perm_t;

// IPC Modes
typedef enum {
	IPC_NOYIELD = 0x0, // Non-Yielding Synchronous
	IPC_YIELD = 0x1,   // Yielding Synchronous
} ipc_mode_t;

// IPC Permissions
typedef enum {
	IPC_SDATA = 0x1, // Server can send data
	IPC_SCAP = 0x2,	 // Server can send capabilities
	IPC_CDATA = 0x4, // Client can send data
	IPC_CCAP = 0x8,	 // Client can send capabilities
} ipc_perm_t;

// Capability types
typedef enum capty {
	CAPTY_NONE = 0,	   ///< No capability.
	CAPTY_TIME = 1,	   ///< Time Slice capability.
	CAPTY_MEMORY = 2,  ///< Memory Slice capability.
	CAPTY_PMP = 3,	   ///< PMP Frame capability.
	CAPTY_MONITOR = 4, ///< Monitor capability.
	CAPTY_CHANNEL = 5, ///< IPC Channel capability.
	CAPTY_SOCKET = 6,  ///< IPC Socket capability.
	CAPTY_COUNT	   ///< Number of capability types
} capty_t;

/// Capability description
typedef union cap {
	capty_t type : 4;

	uint64_t raw;

	struct {
		capty_t type : 4;
		uint16_t _padding : 4;
		hart_t hart;
		time_slot_t bgn;
		time_slot_t mrk;
		time_slot_t end;
	} time;

	struct {
		capty_t type : 4;
		rwx_t rwx : 3;
		bool lck : 1;
		tag_t tag;
		block_t bgn;
		block_t mrk;
		block_t end;
	} mem;

	struct {
		capty_t type : 4;
		rwx_t rwx : 3;
		bool used : 1;
		pmp_slot_t slot;
		napot_t addr : 32;
	} pmp;

	struct {
		capty_t type : 4;
		uint16_t _padding : 12;
		pid_t bgn;
		pid_t mrk;
		pid_t end;
	} mon;

	struct {
		capty_t type : 4;
		uint16_t _padding : 12;
		chan_t bgn;
		chan_t mrk;
		chan_t end;
	} chan;

	struct {
		capty_t type : 4;
		ipc_mode_t mode : 4;
		ipc_perm_t perm : 8;
		chan_t chan;
		uint32_t tag;
	} sock;
} cap_t;

_Static_assert(sizeof(cap_t) == 8, "cap_t has the wrong size");
