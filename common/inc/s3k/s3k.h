#ifndef S3K_H
#define S3K_H

#include <stdint.h>

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
		uint64_t base : 16;
		uint64_t alloc : 16;
		uint64_t size : 16;
	} time;

	struct {
		uint64_t type : 4;
		uint64_t rwx : 3;
		uint64_t lock : 1;
		uint64_t base : 24;
		uint64_t alloc : 16;
		uint64_t size : 16;
	} memory;

	struct {
		uint64_t type : 4;
		uint64_t rwx : 3;
		uint64_t used : 1;
		uint64_t index : 8;
		uint64_t addr : 48;
	} pmp;

	struct {
		uint64_t type : 4;
		uint64_t unused : 12;
		uint64_t base : 16;
		uint64_t alloc : 16;
		uint64_t size : 16;
	} monitor;

	struct {
		uint64_t type : 4;
		uint64_t unused : 12;
		uint64_t base : 16;
		uint64_t alloc : 16;
		uint64_t size : 16;
	} channel;

	struct {
		uint64_t type : 4;
		uint64_t mode : 4;
		uint64_t perm : 8;
		uint64_t channel : 16;
		uint64_t tag : 32;
	} socket;
} s3k_cap_t;

typedef enum {
	SUCCESS,
	ERR_EMPTY,
	ERR_SRC_EMPTY,
	ERR_DST_OCCUPIED,
	ERR_INVALID_CLIENT,
	ERR_INVALID_DERIVATION,
	ERR_INVALID_INDEX,
	ERR_INVALID_MONITOR,
	ERR_INVALID_PID,
	ERR_INVALID_PMP,
	ERR_INVALID_SERVER,
	ERR_INVALID_SLOT,
	ERR_INVALID_STATE,
	ERR_INVALID_SYSCALL,
	ERR_NO_RECEIVER,
	ERR_PREEMPTED,
	ERR_SUSPENDED,
} s3k_error_t;

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

/**
 * Construct a time slice capability.
 *
 * @param hart  Hardware thread ID.
 * @param base  Base of the time slice.
 * @param size  Size of the time slice.
 * @return      Initialized time capability.
 */
static inline s3k_cap_t s3k_time_cap(uint64_t hart, uint64_t base, uint64_t size)
{
	s3k_cap_t cap;
	cap.type = S3K_CAPTY_TIME;
	cap.time.base = base;
	cap.time.alloc = 0;
	cap.time.size = size;
	return cap;
}

/**
 * Construct a memory slice capability.
 *
 * @param base  Base memory address in 4KB.
 * @param size  Size of the memory in 4KB.
 * @param rwx   Read-write-execute permissions encoded as bit flags.
 * @return      Initialized memory capability.
 */
static inline s3k_cap_t s3k_memory_cap(uint64_t base, uint64_t size, uint64_t rwx)
{
	s3k_cap_t cap;
	cap.type = S3K_CAPTY_MEMORY;
	cap.memory.base = base;
	cap.memory.alloc = 0;
	cap.memory.size = size;
	cap.memory.rwx = rwx;
	cap.memory.lock = 0;
	return cap;
}

/**
 * Construct a PMP (Physical Memory Protection) capability using NAPOT mode.
 *
 * @param addr  Base address of the naturally aligned power-of-two region. The
 * alignment should adhere to the constraints set by NAPOT mode.
 * @param rwx   Read-write-execute permissions encoded as bit flags for the
 * region.
 * @return      Initialized PMP capability.
 */
static inline s3k_cap_t s3k_pmp_cap(uint64_t addr, uint64_t rwx)
{
	s3k_cap_t cap;
	cap.type = S3K_CAPTY_PMP;
	cap.pmp.addr = addr;
	cap.pmp.rwx = rwx;
	cap.pmp.used = 0;
	cap.pmp.index = 0;
	return cap;
}

/**
 * Construct a monitor slice capability.
 *
 * @param base   Base of for the monitor.
 * @param size   Size of the monitor capability.
 * @return       Initialized monitor capability.
 */
static inline s3k_cap_t s3k_monitor_cap(uint64_t base, uint64_t size)
{
	s3k_cap_t cap;
	cap.type = S3K_CAPTY_MONITOR;
	cap.monitor.base = base;
	cap.monitor.size = size;
	return cap;
}

/**
 * Construct an IPC channel slice capability.
 *
 * @param base   Base of the IPC channel.
 * @param size   Size of the IPC channel capability.
 * @return       Initialized IPC channel capability.
 */
static inline s3k_cap_t s3k_channel_cap(uint64_t base, uint64_t size)
{
	s3k_cap_t cap;
	cap.type = S3K_CAPTY_CHANNEL;
	cap.channel.base = base;
	cap.channel.size = size;
	return cap;
}

/**
 * Construct an IPC socket capability.
 *
 * Sockets are of two main types: server and client. Server sockets have a tag
 * of 0, while client sockets have a non-zero tag. Depending on the mode
 * (yielding, non-yielding, or asynchronous), there can be a transfer or
 * donation of execution time. Specifically:
 * - Yielding: Upon sending a message, the sender donates its remaining
 * execution time to the receiver.
 * - Non-Yielding: Standard communication without time donation.
 * - Asynchronous: The client donates its execution time to the server
 * asynchronously.
 *
 * @param channel  Channel identifier or address for the IPC socket.
 * @param mode     Communication mode for the IPC socket.
 * @param perm     Permissions associated with the IPC socket, determining the
 * behavior of both server and client:
 *                 - SERVER_DATA: Server can send data, and the client is
 * expected to receive data.
 *                 - SERVER_CAP: Server can send capabilities, and the client is
 * expected to receive capabilities.
 *                 - CLIENT_DATA: Client can send data, and the server is
 * expected to receive data.
 *                 - CLIENT_CAP: Client can send capabilities, and the server is
 * expected to receive capabilities.
 * @param tag      Tag or type identifier for the IPC socket. A tag of 0
 * indicates a server socket, whereas a non-zero tag indicates a client socket.
 * @return         Initialized IPC socket capability.
 */
static inline s3k_cap_t s3k_socket_cap(uint64_t channel, uint64_t mode, uint64_t perm, uint64_t tag)
{
	s3k_cap_t cap;
	cap.type = S3K_CAPTY_SOCKET;
	cap.socket.channel = channel;
	cap.socket.mode = mode;
	cap.socket.perm = perm;
	cap.socket.tag = tag;
	return cap;
}

// Utility functions
static inline void s3k_napot_decode(uint64_t addr, uint64_t *base, uint64_t *size)
{
	*base = ((addr + 1) & addr) << 2;
	*size = (((addr + 1) ^ addr) + 1) << 2;
}

static inline uint64_t s3k_napot_encode(uint64_t base, uint64_t size)
{
	return (base | (size / 2 - 1)) >> 2;
}

static inline int s3k_is_parent(s3k_cap_t a, s3k_cap_t b)
{
	if (a.type == S3K_CAPTY_TIME && b.type == S3K_CAPTY_TIME) {
		return a.time.hart == b.time.hart && a.time.base <= b.time.base
		       && (b.time.base + b.time.size) <= (a.time.base + a.time.size);
	}

	if (a.type == S3K_CAPTY_MEMORY && b.type == S3K_CAPTY_MEMORY) {
		return a.memory.base <= b.memory.base
		       && (b.memory.base + b.memory.size) <= (a.memory.base + a.memory.size)
		       && (a.memory.rwx & b.memory.rwx) == b.memory.rwx;
	}

	if (a.type == S3K_CAPTY_MEMORY && b.type == S3K_CAPTY_PMP) {
		uint64_t mem_base = a.memory.base << 12;
		uint64_t mem_size = a.memory.size << 12;
		uint64_t pmp_begin, pmp_end;
		s3k_napot_decode(b.pmp.addr, &pmp_begin, &pmp_end);
		return mem_base <= pmp_begin && pmp_end <= (mem_base + mem_size)
		       && (a.memory.rwx & b.pmp.rwx) == b.pmp.rwx;
	}

	if (a.type == S3K_CAPTY_MONITOR && b.type == S3K_CAPTY_MONITOR) {
		return a.monitor.base <= b.monitor.base
		       && (b.monitor.base + b.monitor.size) <= (a.monitor.base + a.monitor.size);
	}

	if (a.type == S3K_CAPTY_CHANNEL && b.type == S3K_CAPTY_CHANNEL) {
		return a.channel.base <= b.channel.base
		       && (b.channel.base + b.channel.size) <= (a.channel.base + a.channel.size);
	}

	if (a.type == S3K_CAPTY_CHANNEL && b.type == S3K_CAPTY_SOCKET) {
		return a.channel.base <= b.socket.channel && b.socket.channel < (a.channel.base + a.channel.size);
	}

	if (a.type == S3K_CAPTY_SOCKET && b.type == S3K_CAPTY_SOCKET) {
		return a.socket.channel == b.socket.channel && a.socket.mode == b.socket.mode
		       && a.socket.perm == b.socket.perm && a.socket.tag == 0 && b.socket.tag != 0;
	}

	return 0;
}

static inline int s3k_is_derivable(s3k_cap_t a, s3k_cap_t b)
{
	if (a.type == S3K_CAPTY_TIME && b.type == S3K_CAPTY_TIME) {
		return a.time.hart == b.time.hart && (a.time.base + a.time.alloc) == b.time.base
		       && (b.time.base + b.time.size) <= (a.time.base + a.time.size);
	}

	if (a.type == S3K_CAPTY_MEMORY && b.type == S3K_CAPTY_MEMORY) {
		return (a.memory.base + a.time.alloc) == b.memory.base
		       && (b.memory.base + b.memory.size) <= (a.memory.base + a.memory.size) && a.memory.lock == 0
		       && b.memory.lock == 0 && (a.memory.rwx & b.memory.rwx) == b.memory.rwx;
	}

	if (a.type == S3K_CAPTY_MEMORY && b.type == S3K_CAPTY_PMP) {
		uint64_t memory_base = a.memory.base << 12;
		uint64_t memory_alloc = a.memory.alloc << 12;
		uint64_t memory_size = a.memory.size << 12;
		uint64_t pmp_begin, pmp_end;
		s3k_napot_decode(b.pmp.addr, &pmp_begin, &pmp_end);
		return (memory_base + memory_alloc) <= pmp_begin && pmp_end <= (memory_base + memory_size)
		       && (a.memory.rwx & b.pmp.rwx) == b.pmp.rwx;
	}

	if (a.type == S3K_CAPTY_MONITOR && b.type == S3K_CAPTY_MONITOR) {
		return (a.monitor.base + a.monitor.alloc) == b.monitor.base
		       && (b.monitor.base + b.monitor.size) <= (a.monitor.base + a.monitor.size);
	}

	if (a.type == S3K_CAPTY_CHANNEL && b.type == S3K_CAPTY_CHANNEL) {
		return (a.channel.base + a.channel.alloc) == b.channel.base
		       && (b.channel.base + b.channel.size) <= (a.channel.base + a.channel.size);
	}

	if (a.type == S3K_CAPTY_CHANNEL && b.type == S3K_CAPTY_SOCKET) {
		return (a.channel.base + a.channel.alloc) == b.socket.channel
		       && b.socket.channel < (a.channel.base + a.channel.size) && b.socket.tag == 0;
	}

	if (a.type == S3K_CAPTY_SOCKET && b.type == S3K_CAPTY_SOCKET) {
		return a.socket.channel == b.socket.channel && a.socket.mode == b.socket.mode
		       && a.socket.perm == b.socket.perm && a.socket.tag == 0 && b.socket.tag != 0;
	}

	return 0;
}

// System calls
static inline uint64_t s3k_get_pid(void)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_GET_INFO;
	register uint64_t a0 __asm__("a0") = 0;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(t0));
	return a0;
}

static inline uint64_t s3k_get_time(void)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_GET_INFO;
	register uint64_t a0 __asm__("a0") = 1;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(t0));
	return a0;
}

static inline uint64_t s3k_get_timeout(void)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_GET_INFO;
	register uint64_t a0 __asm__("a0") = 2;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(t0));
	return a0;
}

static inline uint64_t s3k_read_reg(uint64_t reg)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_REG_READ;
	register uint64_t a0 __asm__("a0") = reg;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(t0));
	return a0;
}

static inline void s3k_write_reg(uint64_t reg, uint64_t val)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_REG_WRITE;
	register uint64_t a0 __asm__("a0") = reg;
	register uint64_t a1 __asm__("a1") = val;
	__asm__ volatile("ecall" ::"r"(a0), "r"(a1), "r"(t0));
}

static inline void s3k_sync(void)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_SYNC;
	__asm__ volatile("ecall" ::"r"(t0));
}

static inline void s3k_sync_mem(void)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_SYNC_MEM;
	__asm__ volatile("ecall" ::"r"(t0));
}

static inline s3k_error_t s3k_cap_read(uint64_t read_idx, s3k_cap_t *cap)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_CAP_READ;
	register uint64_t a0 __asm__("a0") = read_idx;
	__asm__ volatile("ecall" : "+r"(t0), "+r"(a0));
	if (!a0)
		cap->raw = a0;
	return t0;
}

static inline s3k_error_t s3k_cap_move(uint64_t src_idx, uint64_t dst_idx)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_CAP_MOVE;
	register uint64_t a0 __asm__("a0") = src_idx;
	register uint64_t a1 __asm__("a1") = dst_idx;
	__asm__ volatile("ecall" : "+r"(t0) : "r"(a0), "r"(a1));
	return t0;
}

static inline s3k_error_t s3k_cap_delete(uint64_t del_idx)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_CAP_DELETE;
	register uint64_t a0 __asm__("a0") = del_idx;
	__asm__ volatile("ecall" : "+r"(t0) : "r"(a0));
	return t0;
}

static inline s3k_error_t s3k_cap_revoke(uint64_t rev_idx)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_CAP_REVOKE;
	register uint64_t a0 __asm__("a0") = rev_idx;
	__asm__ volatile("ecall" : "+r"(t0) : "r"(a0));
	return t0;
}

static inline s3k_error_t s3k_cap_derive(uint64_t src_idx, uint64_t dst_idx, s3k_cap_t new_cap)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_CAP_DERIVE;
	register uint64_t a0 __asm__("a0") = src_idx;
	register uint64_t a1 __asm__("a1") = dst_idx;
	register uint64_t a2 __asm__("a2") = new_cap.raw;
	__asm__ volatile("ecall" : "+r"(t0) : "r"(a0), "r"(a1), "r"(a2));
	return t0;
}

static inline s3k_error_t s3k_pmp_load(uint64_t pmp_idx, uint64_t pmp_slot)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_PMP_LOAD;
	register uint64_t a0 __asm__("a0") = pmp_idx;
	register uint64_t a1 __asm__("a1") = pmp_slot;
	__asm__ volatile("ecall" : "+r"(t0) : "r"(a0), "r"(a1));
	return t0;
}

static inline s3k_error_t s3k_pmp_unload(uint64_t pmp_idx)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_PMP_UNLOAD;
	register uint64_t a0 __asm__("a0") = pmp_idx;
	__asm__ volatile("ecall" : "+r"(t0) : "r"(a0));
	return t0;
}

static inline s3k_error_t s3k_mon_suspend(uint64_t mon_idx, uint64_t pid)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_MON_SUSPEND;
	register uint64_t a0 __asm__("a0") = mon_idx;
	register uint64_t a1 __asm__("a1") = pid;
	__asm__ volatile("ecall" : "+r"(t0) : "r"(a0), "r"(a1));
	return t0;
}

static inline s3k_error_t s3k_mon_resume(uint64_t mon_idx, uint64_t pid)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_MON_RESUME;
	register uint64_t a0 __asm__("a0") = mon_idx;
	register uint64_t a1 __asm__("a1") = pid;
	__asm__ volatile("ecall" : "+r"(t0) : "r"(a0), "r"(a1));
	return t0;
}

static inline s3k_error_t s3k_mon_reg_read(uint64_t mon_idx, uint64_t pid, uint64_t reg, uint64_t *val)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_MON_REG_READ;
	register uint64_t a0 __asm__("a0") = mon_idx;
	register uint64_t a1 __asm__("a1") = pid;
	register uint64_t a2 __asm__("a2") = reg;
	__asm__ volatile("ecall" : "+r"(t0), "+r"(a0) : "r"(a1), "r"(a2));
	*val = a0;
	return t0;
}

static inline s3k_error_t s3k_mon_reg_write(uint64_t mon_idx, uint64_t pid, uint64_t reg, uint64_t val)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_MON_REG_WRITE;
	register uint64_t a0 __asm__("a0") = mon_idx;
	register uint64_t a1 __asm__("a1") = pid;
	register uint64_t a2 __asm__("a2") = reg;
	register uint64_t a3 __asm__("a3") = val;
	__asm__ volatile("ecall" : "+r"(t0) : "r"(a0), "r"(a1), "r"(a2), "r"(a3));
	return t0;
}

static inline s3k_error_t s3k_mon_cap_read(uint64_t mon_idx, uint64_t pid, uint64_t read_idx, s3k_cap_t *cap)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_MON_CAP_READ;
	register uint64_t a0 __asm__("a0") = mon_idx;
	register uint64_t a1 __asm__("a1") = pid;
	register uint64_t a2 __asm__("a2") = read_idx;
	__asm__ volatile("ecall" : "+r"(t0), "+r"(a0) : "r"(a1), "r"(a2));
	if (!t0)
		cap->raw = a0;
	return t0;
}

static inline s3k_error_t s3k_mon_cap_move(uint64_t mon_idx, uint64_t src_pid, uint64_t src_idx, uint64_t dst_pid,
					   uint64_t dst_idx)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_MON_CAP_MOVE;
	register uint64_t a0 __asm__("a0") = mon_idx;
	register uint64_t a1 __asm__("a1") = src_pid;
	register uint64_t a2 __asm__("a2") = src_idx;
	register uint64_t a3 __asm__("a3") = dst_pid;
	register uint64_t a4 __asm__("a4") = dst_idx;
	__asm__ volatile("ecall" : "+r"(t0) : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4));
	return t0;
}

static inline s3k_error_t s3k_mon_pmp_load(uint64_t mon_idx, uint64_t pid, uint64_t pmp_idx, uint64_t pmp_slot)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_MON_PMP_LOAD;
	register uint64_t a0 __asm__("a0") = mon_idx;
	register uint64_t a1 __asm__("a1") = pid;
	register uint64_t a2 __asm__("a2") = pmp_idx;
	register uint64_t a3 __asm__("a3") = pmp_slot;
	__asm__ volatile("ecall" : "+r"(t0) : "r"(a0), "r"(a1), "r"(a2), "r"(a3));
	return t0;
}

static inline s3k_error_t s3k_mon_pmp_unload(uint64_t mon_idx, uint64_t pid, uint64_t pmp_idx)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_MON_PMP_UNLOAD;
	register uint64_t a0 __asm__("a0") = mon_idx;
	register uint64_t a1 __asm__("a1") = pid;
	register uint64_t a2 __asm__("a2") = pmp_idx;
	__asm__ volatile("ecall" : "+r"(t0) : "r"(a0), "r"(a1), "r"(a2));
	return t0;
}

#endif /* S3K_H */
