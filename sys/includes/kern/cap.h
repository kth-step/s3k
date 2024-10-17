#pragma once

#include "kern/types.h"
#include "kern/error.h"

// Min logarithmic size of a memory slice
#define MIN_BLOCK_SIZE 12

// Max logarithmic size of a memory slice
#define MAX_BLOCK_SIZE 27

typedef enum mem_perm mem_perm_t;
typedef enum ipc_mode ipc_mode_t;
typedef enum ipc_perm ipc_perm_t;
typedef enum capty capty_t;
typedef struct cap cap_t;
typedef union cap_data cap_data_t;
typedef uint32_t cidx_t;

enum mem_perm {
	MEM_NONE = 0,
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
	CAPTY_SOCKET = 6,  ///< IPC Socket capability.
	CAPTY_COUNT	   ///< Number of capability types
};

union cap_data {
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
		napot_t addr : 48;
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
};

/// Capability description
struct cap {
	cidx_t prev, next;
	union cap_data data;
};

_Static_assert(sizeof(cap_t) == 16, "cap_t has the wrong size");

static inline cidx_t cap_idx(pid_t pid, reg_t reg)
{
	return pid * S3K_CAP_CNT + reg;
}

static inline addr_t tag_block_to_addr(tag_t tag, block_t block)
{
	return ((uint64_t)tag << MAX_BLOCK_SIZE)
	       + ((uint64_t)block << MIN_BLOCK_SIZE);
}

static inline void pmp_napot_decode(uint64_t addr, uint64_t *base,
				    uint64_t *size)
{
	*base = ((addr + 1) & addr) << 2;
	*size = (((addr + 1) ^ addr) + 1) << 2;
}

static inline uint64_t pmp_napot_encode(uint64_t base, uint64_t size)
{
	return (base | (size / 2 - 1)) >> 2;
}

#define CAP_PMP(_addr, _rwx) 0
#define CAP_MEMORY(_tag, _begin, _end, _rwx) 0
#define CAP_MONITOR(_begin, _end) 0
#define CAP_CHANNEL(_begin, _end) 0
#define CAP_TIME(_begin, _end) 0
#define CAP_NULL ((union cap_data){ .raw = 0 })

err_t cap_read(cidx_t c, cap_data_t *buf);
err_t cap_move(cidx_t src, cidx_t dst);
err_t cap_delete(cidx_t c);
err_t cap_revoke(cidx_t c);
err_t cap_derive(cidx_t src, cidx_t dst, cap_data_t cdata);
