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
typedef uint64_t s3k_state_t;
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
	S3K_SUCCESS = 0,
	S3K_ERR_EMPTY,
	S3K_ERR_SRC_EMPTY,
	S3K_ERR_DST_OCCUPIED,
	S3K_ERR_INVALID_INDEX,
	S3K_ERR_INVALID_DERIVATION,
	S3K_ERR_INVALID_MONITOR,
	S3K_ERR_INVALID_PID,
	S3K_ERR_INVALID_STATE,
	S3K_ERR_INVALID_PMP,
	S3K_ERR_INVALID_SLOT,
	S3K_ERR_INVALID_SOCKET,
	S3K_ERR_INVALID_SYSCALL,
	S3K_ERR_INVALID_REGISTER,
	S3K_ERR_NO_RECEIVER,
	S3K_ERR_PREEMPTED,
	S3K_ERR_TIMEOUT,
	S3K_ERR_SUSPENDED,
} s3k_err_t;

typedef enum {
	S3K_PSF_BUSY = 0x1,
	S3K_PSF_BLOCKED = 0x2,
	S3K_PSF_SUSPENDED = 0x4,
} s3k_state_flag_t;

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

typedef enum {
	S3K_REG_PC,
	S3K_REG_RA,
	S3K_REG_SP,
	S3K_REG_GP,
	S3K_REG_TP,
	S3K_REG_T0,
	S3K_REG_T1,
	S3K_REG_T2,
	S3K_REG_S0,
	S3K_REG_S1,
	S3K_REG_A0,
	S3K_REG_A1,
	S3K_REG_A2,
	S3K_REG_A3,
	S3K_REG_A4,
	S3K_REG_A5,
	S3K_REG_A6,
	S3K_REG_A7,
	S3K_REG_S2,
	S3K_REG_S3,
	S3K_REG_S4,
	S3K_REG_S5,
	S3K_REG_S6,
	S3K_REG_S7,
	S3K_REG_S8,
	S3K_REG_S9,
	S3K_REG_S10,
	S3K_REG_S11,
	S3K_REG_T3,
	S3K_REG_T4,
	S3K_REG_T5,
	S3K_REG_T6,
	S3K_REG_TPC,
	S3K_REG_TSP,
	S3K_REG_EPC,
	S3K_REG_ESP,
	S3K_REG_ECAUSE,
	S3K_REG_EVAL,
	S3K_REG_SERVTIME,
	S3K_REG_WCET,
	/* Special value for number of registers */
	S3K_REG_CNT,
} s3k_reg_t;

typedef struct {
	s3k_cidx_t cap_idx;
	bool send_cap;
	uint64_t data[4];
} s3k_msg_t;

typedef struct {
	s3k_err_t err;
	uint32_t tag;
	s3k_cap_t cap;
	uint64_t data[4];
} s3k_reply_t;
