#pragma once

#include "kern/error.h"
#include "kern/types.h"

// Min logarithmic size of a memory slice
#define MIN_BLOCK_SIZE 12

// Max logarithmic size of a memory slice
#define MAX_BLOCK_SIZE 27

enum mem_perm {
	MEM_R = 0x1,
	MEM_W = 0x2,
	MEM_X = 0x4,
	MEM_RW = MEM_R | MEM_W,
	MEM_RX = MEM_R | MEM_X,
	MEM_RWX = MEM_R | MEM_W | MEM_X,
};

// IPC Modes
enum ipc_mode {
	IPC_NOYIELD = 0x0, // Non-Yielding Synchronous
	IPC_YIELD = 0x1,   // Yielding Synchronous
};

// IPC Permissions
enum ipc_perm {
	IPC_SDATA = 0x1, // Server can send data
	IPC_SCAP = 0x2,	 // Server can send capabilities
	IPC_CDATA = 0x4, // Client can send data
	IPC_CCAP = 0x8,	 // Client can send capabilities
};

// Capability types
enum capty {
	CAPTY_NONE = 0,	   ///< No capability.
	CAPTY_TIME = 1,	   ///< Time Slice capability.
	CAPTY_MEMORY = 2,  ///< Memory Slice capability.
	CAPTY_PMP = 3,	   ///< PMP Frame capability.
	CAPTY_MONITOR = 4, ///< Monitor capability.
	CAPTY_CHANNEL = 5, ///< IPC Channel capability.
	CAPTY_SERVER = 6,  ///< IPC Socket capability.
	CAPTY_CLIENT = 7,  ///< IPC Socket capability.
	CAPTY_COUNT	   ///< Number of capability types
};

typedef union {
	uint64_t type : 4;

	uint64_t raw;

	struct {
		uint64_t type : 4;
		uint64_t _padding : 4;
		uint64_t hart : 8;
		uint64_t begin : 16;
		uint64_t mark : 16;
		uint64_t end : 16;
	} time;

	struct {
		uint64_t type : 4;
		uint64_t rwx : 3;
		uint64_t lock : 1;
		uint64_t tag : 8;
		uint64_t begin : 16;
		uint64_t mark : 16;
		uint64_t end : 16;
	} mem;

	struct {
		uint64_t type : 4;
		uint64_t rwx : 3;
		uint64_t _padding : 9;
		uint64_t addr : 48;
	} pmp;

	struct {
		uint64_t type : 4;
		uint64_t _padding : 12;
		uint64_t begin : 16;
		uint64_t mark : 16;
		uint64_t end : 16;
	} mon;

	struct {
		uint64_t type : 4;
		uint64_t _padding : 12;
		uint64_t begin : 16;
		uint64_t mark : 16;
		uint64_t end : 16;
	} chan;

	struct {
		uint64_t type : 4;
		uint64_t mode : 4;
		uint64_t permission : 8;
		uint64_t channel : 16;
		uint64_t _padding : 32;
	} server;

	struct {
		uint64_t type : 4;
		uint64_t mode : 4;
		uint64_t permission : 8;
		uint64_t channel : 16;
		uint64_t tag : 32;
	} client;
} Cap;

/// Capability description

_Static_assert(sizeof(Cap) == 8, "cap_t has the wrong size");

static inline Word CapIdx(Pid pid, Word reg)
{
	return pid * S3K_CAP_CNT + reg;
}

static inline uint64_t pmp_napot_decode_end(uint64_t addr)
{
	return pmp_napot_decode_bgn(addr) + pmp_napot_decode_size(addr);
}

static inline void pmp_napot_decode(uint64_t addr, uint64_t *bgn, uint64_t *end)
{
	*bgn = pmp_napot_decode_bgn(addr);
	*end = pmp_napot_decode_end(addr);
}

static inline uint64_t pmp_napot_encode(uint64_t bgn, uint64_t end)
{
	return (bgn | ((end - bgn) / 2 - 1)) >> 2;
}

#define CAP_PMP(_addr, _rwx) 0
#define CAP_MEMORY(_tag, _begin, _end, _rwx) 0
#define CAP_MONITOR(_begin, _end) 0
#define CAP_CHANNEL(_begin, _end) 0
#define CAP_TIME(_begin, _end) 0
#define CAP_NULL ((Cap){.raw = 0})

Word CapRead(Word i, Cap *cap);
Word CapMove(Word src, Word dst);
Word CapDelete(Word i);
Word CapRevoke(Word i);
Word CapDerive(Word src, Word dst, Cap cap);
