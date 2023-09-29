#ifndef S3K_H
#define S3K_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
	S3K_MEM_NONE = 0,
	S3K_MEM_R = 1,
	S3K_MEM_W = 2,
	S3K_MEM_X = 4,
	S3K_MEM_RW = S3K_MEM_R | S3K_MEM_W,
	S3K_MEM_RX = S3K_MEM_R | S3K_MEM_X,
	S3K_MEM_RWX = S3K_MEM_R | S3K_MEM_W | S3K_MEM_X,
} s3k_mem_perm_t;

// IPC Modes
typedef enum {
	S3K_IPC_NOYIELD = 0, // Non-Yielding Synchronous
	S3K_IPC_YIELD = 1,   // Yielding Synchronous
			     //	S3K_IPC_ASYNC = 2,   // Asynchronous
} s3k_ipc_mode_t;

// IPC Permissions
typedef enum {
	S3K_IPC_SDATA = 1, // Server can send data
	S3K_IPC_SCAP = 2,  // Server can send capabilities
	S3K_IPC_CDATA = 4, // Client can send data
	S3K_IPC_CCAP = 8,  // Client can send capabilities
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
} s3k_cap_t;

typedef enum {
	S3K_SUCCESS,
	S3K_ERR_EMPTY,
	S3K_ERR_SRC_EMPTY,
	S3K_ERR_DST_OCCUPIED,
	S3K_ERR_INVALID_CLIENT,
	S3K_ERR_INVALID_DERIVATION,
	S3K_ERR_INVALID_INDEX,
	S3K_ERR_INVALID_MONITOR,
	S3K_ERR_INVALID_PID,
	S3K_ERR_INVALID_PMP,
	S3K_ERR_INVALID_SERVER,
	S3K_ERR_INVALID_SLOT,
	S3K_ERR_INVALID_STATE,
	S3K_ERR_INVALID_SYSCALL,
	S3K_ERR_NO_RECEIVER,
	S3K_ERR_PREEMPTED,
	S3K_ERR_SUSPENDED,
} s3k_err_t;

typedef enum {
	// Basic Info & Registers
	S3K_SYSCALL_GET_INFO,  // Retrieve system information
	S3K_SYSCALL_REG_READ,  // Read from a register
	S3K_SYSCALL_REG_WRITE, // Write to a register
	S3K_SYSCALL_SYNC,      // Synchronize memory and time.
	S3K_SYSCALL_SYNC_MEM,  // Synchronize memory.

	// Capability Management
	S3K_SYSCALL_CAP_READ,	// Read the properties of a capability.
	S3K_SYSCALL_CAP_MOVE,	// Move a capability to a different slot.
	S3K_SYSCALL_CAP_DELETE, // Delete a capability from the system.
	S3K_SYSCALL_CAP_REVOKE, // Deletes derived capabilities.
	S3K_SYSCALL_CAP_DERIVE, // Creates a new capability.

	// PMP calls
	S3K_SYSCALL_PMP_LOAD,
	S3K_SYSCALL_PMP_UNLOAD,

	// Monitor calls
	S3K_SYSCALL_MON_SUSPEND,
	S3K_SYSCALL_MON_RESUME,
	S3K_SYSCALL_MON_REG_READ,
	S3K_SYSCALL_MON_REG_WRITE,
	S3K_SYSCALL_MON_CAP_READ,
	S3K_SYSCALL_MON_CAP_MOVE,
	S3K_SYSCALL_MON_PMP_LOAD,
	S3K_SYSCALL_MON_PMP_UNLOAD,

	// Socket calls
	S3K_SYSCALL_SOCK_SEND,
	S3K_SYSCALL_SOCK_CALL,
	S3K_SYSCALL_SOCK_REPLY,
	S3K_SYSCALL_SOCK_RECV,
	S3K_SYSCALL_SOCK_REPLYRECV,
} s3k_syscall_t;

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
	S3K_REG_EVAL
} s3k_reg_t;


s3k_cap_t s3k_mk_time(uint64_t hart, uint64_t bgn, uint64_t end);
s3k_cap_t s3k_mk_memory(uint64_t bgn, uint64_t end, uint64_t rwx);
s3k_cap_t s3k_mk_pmp(uint64_t addr, uint64_t rwx);
s3k_cap_t s3k_mk_monitor(uint64_t bgn, uint64_t end);
s3k_cap_t s3k_mk_channel(uint64_t bgn, uint64_t end);
s3k_cap_t s3k_mk_socket(uint64_t chan, uint64_t mode, uint64_t perm, uint64_t tag);

void preempt_set_mask(uint64_t mask);
uint64_t preempt_get_mask(void);
void s3k_napot_decode(uint64_t addr, uint64_t *base, uint64_t *size);
uint64_t s3k_napot_encode(uint64_t base, uint64_t size);

bool s3k_is_parent(s3k_cap_t a, s3k_cap_t b);
bool s3k_is_derivable(s3k_cap_t a, s3k_cap_t b);

// System calls
uint64_t s3k_get_pid(void);
uint64_t s3k_get_time(void);
uint64_t s3k_get_timeout(void);
uint64_t s3k_read_reg(uint64_t reg);
void s3k_write_reg(uint64_t reg, uint64_t val);
void s3k_sync(void);
void s3k_sync_mem(void);
s3k_err_t s3k_cap_read(uint64_t read_idx, s3k_cap_t *cap);
s3k_err_t s3k_cap_move(uint64_t src_idx, uint64_t dst_idx);
s3k_err_t s3k_cap_delete(uint64_t del_idx);
s3k_err_t s3k_cap_revoke(uint64_t rev_idx);
s3k_err_t s3k_cap_derive(uint64_t src_idx, uint64_t dst_idx, s3k_cap_t new_cap);
s3k_err_t s3k_pmp_load(uint64_t pmp_idx, uint64_t pmp_slot);
s3k_err_t s3k_pmp_unload(uint64_t pmp_idx);
s3k_err_t s3k_mon_suspend(uint64_t mon_idx, uint64_t pid);
s3k_err_t s3k_mon_resume(uint64_t mon_idx, uint64_t pid);
s3k_err_t s3k_mon_reg_read(uint64_t mon_idx, uint64_t pid, uint64_t reg, uint64_t *val);
s3k_err_t s3k_mon_reg_write(uint64_t mon_idx, uint64_t pid, uint64_t reg, uint64_t val);
s3k_err_t s3k_mon_cap_read(uint64_t mon_idx, uint64_t pid, uint64_t read_idx, s3k_cap_t *cap);
s3k_err_t s3k_mon_cap_move(uint64_t mon_idx, uint64_t src_pid, uint64_t src_idx, uint64_t dst_pid,
		uint64_t dst_idx);
s3k_err_t s3k_mon_pmp_load(uint64_t mon_idx, uint64_t pid, uint64_t pmp_idx, uint64_t pmp_slot);
s3k_err_t s3k_mon_pmp_unload(uint64_t mon_idx, uint64_t pid, uint64_t pmp_idx);

#endif /* S3K_H */
