#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Min logarithmic size of a memory slice
#define S3K_MIN_BLOCK_SIZE 12
// Max logarithmic size of a memory slice
#define S3K_MAX_BLOCK_SIZE 27

typedef uint64_t s3k_napot_t;
typedef uint64_t s3k_addr_t;
typedef uint16_t s3k_block_t;
typedef uint16_t s3k_chan_t;
typedef uint16_t s3k_time_slot_t;
typedef uint16_t s3k_pid_t;
typedef uint16_t s3k_cidx_t;
typedef uint8_t s3k_hart_t;
typedef uint8_t s3k_tag_t;
typedef uint8_t s3k_rwx_t;
typedef uint8_t s3k_pmp_slot_t;

typedef enum {
	S3K_MEM_NONE = 0,
	S3K_MEM_R = 0x1,
	S3K_MEM_W = 0x2,
	S3K_MEM_X = 0x4,
	S3K_MEM_RW = S3K_MEM_R | S3K_MEM_W,
	S3K_MEM_RX = S3K_MEM_R | S3K_MEM_X,
	S3K_MEM_RWX = S3K_MEM_R | S3K_MEM_W | S3K_MEM_X,
} s3k_mem_perm_t;

// IPC Modes
typedef enum {
	S3K_IPC_NOYIELD = 0x0, // Non-Yielding Synchronous
	S3K_IPC_YIELD = 0x1,   // Yielding Synchronous
			     //	S3K_IPC_ASYNC = 2,   // Asynchronous
} s3k_ipc_mode_t;

// IPC Permissions
typedef enum {
	S3K_IPC_SDATA = 0x1, // Server can send data
	S3K_IPC_SCAP = 0x2,  // Server can send capabilities
	S3K_IPC_CDATA = 0x4, // Client can send data
	S3K_IPC_CCAP = 0x8,  // Client can send capabilities
} s3k_ipc_perm_t;

// Capability types
typedef enum s3k_capty {
	S3K_CAPTY_NONE = 0,    ///< No capability.
	S3K_CAPTY_TIME = 1,    ///< Time Slice capability.
	S3K_CAPTY_MEMORY = 2,  ///< Memory Slice capability.
	S3K_CAPTY_PMP = 3,     ///< PMP Frame capability.
	S3K_CAPTY_MONITOR = 4, ///< Monitor capability.
	S3K_CAPTY_CHANNEL = 5, ///< IPC Channel capability.
	S3K_CAPTY_SOCKET = 6,  ///< IPC Socket capability.
} s3k_capty_t;

/// Capability description
typedef union s3k_cap {
	s3k_capty_t type : 4;

	uint64_t raw;

	struct {
		s3k_capty_t type : 4;
		uint16_t _padding : 4;
		s3k_hart_t hart;
		s3k_time_slot_t bgn;
		s3k_time_slot_t mrk;
		s3k_time_slot_t end;
	} time;

	struct {
		s3k_capty_t type : 4;
		s3k_rwx_t rwx : 3;
		bool lck : 1;
		s3k_tag_t tag;
		s3k_block_t bgn;
		s3k_block_t mrk;
		s3k_block_t end;
	} mem;

	struct {
		s3k_capty_t type : 4;
		s3k_rwx_t rwx : 3;
		bool used : 1;
		s3k_pmp_slot_t slot;
		s3k_napot_t addr : 48;
	} pmp;

	struct {
		s3k_capty_t type : 4;
		uint16_t _padding : 12;
		s3k_pid_t bgn;
		s3k_pid_t mrk;
		s3k_pid_t end;
	} mon;

	struct {
		s3k_capty_t type : 4;
		uint16_t _padding : 12;
		s3k_chan_t bgn;
		s3k_chan_t mrk;
		s3k_chan_t end;
	} chan;

	struct {
		s3k_capty_t type : 4;
		s3k_ipc_mode_t mode : 4;
		s3k_ipc_perm_t perm : 8;
		s3k_chan_t chan;
		uint32_t tag;
	} sock;
} s3k_cap_t;

_Static_assert(sizeof(s3k_cap_t) == 8, "s3k_cap_t has the wrong size");

s3k_cap_t s3k_mk_time(s3k_hart_t hart, s3k_time_slot_t bgn,
		      s3k_time_slot_t end);
s3k_cap_t s3k_mk_memory(s3k_addr_t bgn, s3k_addr_t end, s3k_rwx_t rwx);
s3k_cap_t s3k_mk_pmp(s3k_napot_t napot_addr, s3k_rwx_t rwx);
s3k_cap_t s3k_mk_monitor(s3k_pid_t bgn, s3k_pid_t end);
s3k_cap_t s3k_mk_channel(s3k_chan_t bgn, s3k_chan_t end);
s3k_cap_t s3k_mk_socket(s3k_chan_t chan, s3k_ipc_mode_t mode,
			s3k_ipc_perm_t perm, uint32_t tag);

bool s3k_is_valid(s3k_cap_t a);
bool s3k_is_parent(s3k_cap_t a, s3k_cap_t b);
bool s3k_is_derivable(s3k_cap_t a, s3k_cap_t b);
