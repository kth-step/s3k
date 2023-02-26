/* See LICENSE file for copyright and license details. */
/**
 * @file s3k.h
 * @brief Kernel API for user applications.
 *
 * This file contains all necessary enums, structs, and functions used for interacting with the S3K
 * Kernel.
 *
 * @author Henrik Karlsson (henrik10@kth.se)
 */
#ifndef __S3K_H__
#define __S3K_H__
#include <stdbool.h>
#include <stdint.h>

/// @defgroup api S3K Kernel API
///
/// S3K Kernel User-level API
///
/// @{

#define S3K_RWX 0x7  ///< Read, write and execute for Memory and PMP capability
#define S3K_RW	0x3  ///< Read and write permissions for memory and PMP capability
#define S3K_RX	0x5  ///< Read and execute permissions for memory and PMP capability
#define S3K_R	0x1  ///< Read permissions for memory and PMP capability

/**
 * @brief Enumeration S3K system call exception codes.
 *
 * These exeception codes are returned by most of S3K's system calls. When no exception occured
 * while executing a system call, `S3K_EXCPT_NONE` (=0) is returned.
 */
enum s3k_excpt {
	S3K_EXCPT_NONE,		 ///< No exception.
	S3K_EXCPT_EMPTY,	 ///< Capability slot is empty.
	S3K_EXCPT_COLLISION,	 ///< Capability slot is occupied.
	S3K_EXCPT_DERIVATION,	 ///< Capability can not be derived.
	S3K_EXCPT_PREEMPTED,	 ///< System call was preempted.
	S3K_EXCPT_SUSPENDED,	 ///< Process was suspended
	S3K_EXCPT_MPID,		 ///< Bad PID for monitor operation.
	S3K_EXCPT_MBUSY,	 ///< Process busy.
	S3K_EXCPT_UNIMPLEMENTED	 ///< System call not implemented for specified capability.
};

/// @brief Enumeration of capability types.
enum s3k_capty {
	/// **No capability**
	S3K_CAPTY_NONE,
	/// **Time slice capability**, letting the process modify and manage the minor frames of the
	/// round-robin scheduler.
	S3K_CAPTY_TIME,
	/// **Memory slice capability** for manage slices of memory. Can be used to derive memory
	/// slice capabilities and PMP capabilities.
	S3K_CAPTY_MEMORY,
	/// **PMP capability**, letting the process modify RISC-V's Physical Memory Protection (PMP)
	/// unit.
	S3K_CAPTY_PMP,
	/// **Process monitor capability**, letting the process suspend, resume and modify suspended
	/// processes.
	S3K_CAPTY_MONITOR,
	/// **IPC channel capability**, for creating and managing IPC channel receive endpoints.
	S3K_CAPTY_CHANNEL,
	/// **IPC receive capability**, IPC endpoint for receiving messages and capabilities. Can be
	/// used to derive IPC send capabilities.
	S3K_CAPTY_SOCKET,
};

/**
 * @brief Enumeration of S3K registers.
 *
 * Includes RISC-V's general purpose registers (GPR), program counter, and S3K specific
 * virtual registers (VR).
 */
enum s3k_reg {
	/* General purpose registers */
	S3K_REG_PC,   ///< Program counter
	S3K_REG_RA,   ///< Return address (GPR)
	S3K_REG_SP,   ///< Stack pointer (GPR)
	S3K_REG_GP,   ///< Global pointer (GPR)
	S3K_REG_TP,   ///< Thread pointer (GPR)
	S3K_REG_T0,   ///< Temporary register (GPR)
	S3K_REG_T1,   ///< Temporary register (GPR)
	S3K_REG_T2,   ///< Temporary register (GPR)
	S3K_REG_S0,   ///< Saved register/Stack frame pointer (GPR)
	S3K_REG_S1,   ///< Saved register (GPR)
	S3K_REG_A0,   ///< Argument/Return register (GPR)
	S3K_REG_A1,   ///< Argument/Return register (GPR)
	S3K_REG_A2,   ///< Argument register (GPR)
	S3K_REG_A3,   ///< Argument register (GPR)
	S3K_REG_A4,   ///< Argument register (GPR)
	S3K_REG_A5,   ///< Argument register (GPR)
	S3K_REG_A6,   ///< Argument register (GPR)
	S3K_REG_A7,   ///< Argument register (GPR)
	S3K_REG_S2,   ///< Saved register (GPR)
	S3K_REG_S3,   ///< Saved register (GPR)
	S3K_REG_S4,   ///< Saved register (GPR)
	S3K_REG_S5,   ///< Saved register (GPR)
	S3K_REG_S6,   ///< Saved register (GPR)
	S3K_REG_S7,   ///< Saved register (GPR)
	S3K_REG_S8,   ///< Saved register (GPR)
	S3K_REG_S9,   ///< Saved register (GPR)
	S3K_REG_S10,  ///< Saved register (GPR)
	S3K_REG_S11,  ///< Saved register (GPR)
	S3K_REG_T3,   ///< Temporary register (GPR)
	S3K_REG_T4,   ///< Temporary register (GPR)
	S3K_REG_T5,   ///< Temporary register (GPR)
	S3K_REG_T6,   ///< Temporary register (GPR)
	/* Virtual registers */
	S3K_REG_CAUSE,	///< Exception cause code.
	S3K_REG_TVAL,	///< Exception value.
	S3K_REG_EPC,	///< Exception program counter.
	S3K_REG_TVEC,	///< Exception handling vector.
	S3K_REG_PMP,	///< PMP configuration.
	S3K_REG_COUNT	///< *Number of S3K registers.*
};

/// Time slice capability
struct s3k_time {
	uint64_t type : 4;    ///< Type of capability, should be S3K_CAPTY_TIME.
	uint64_t _padd : 4;   ///< Padding, should be zero.
	uint64_t hartid : 8;  ///< Hardware Thread ID.
	uint64_t begin : 16;  ///< Beginning of time slice.
	uint64_t free : 16;   ///< Beginning of available/unallocated time slice.
	uint64_t end : 16;    ///< End of time slice.
};

/// Memory Slice capability
struct s3k_memory {
	uint64_t type : 4;    ///< Type of capability, should be S3K_CAPTY_MEMORY
	uint64_t lock : 1;    ///< Prevents creating of memory capabilities
	uint64_t rwx : 3;     ///< Read, write and execute (reverse order)
	uint64_t offset : 8;  ///< 128 MiB offset of memory slice
	uint64_t begin : 16;  ///< Beginning of memory slice
	uint64_t free : 16;   ///< Beginning of available/unallocated memory slice
	uint64_t end : 16;    ///< End of memory slice
};

/// PMP Frame capability
struct s3k_pmp {
	uint64_t type : 4;   ///< Type of capability, should be S3K_CAPTY_PMP
	uint64_t addr : 52;  ///< pmpaddr
	uint64_t cfg : 8;    ///< pmpcfg
};

/// Monitor capability
struct s3k_monitor {
	uint64_t type : 4;    ///< Type of capability, should be S3K_CAPTY_MONITOR
	uint64_t _padd : 12;  ///< Padding, should be zero
	uint64_t begin : 16;  ///< Beginning of monitored PIDs.
	uint64_t free : 16;   ///< Beginning of available monitored PIDs.
	uint64_t end : 16;    ///< End of monitred PIDs
};

/// Channel capability
struct s3k_channel {
	uint64_t type : 4;
	uint64_t _padd : 12;
	uint64_t begin : 16;
	uint64_t free : 16;
	uint64_t end : 16;
};

/// Socket capability
struct s3k_socket {
	uint64_t type : 4;
	uint64_t _padd : 28;
	uint64_t channel : 16;
	uint64_t tag : 16;
};

/// Capability description
union s3k_cap {
	uint64_t type : 4;	     ///< Type of capability
	uint64_t raw;		     ///< Capability as 64-bit word
	struct s3k_time time;	     ///< As time slice capability
	struct s3k_memory memory;    ///< As memory slice capability
	struct s3k_pmp pmp;	     ///< As PMP frame capability
	struct s3k_monitor monitor;  ///< As monitor slice capability
	struct s3k_channel channel;  ///< As channel slice capability
	struct s3k_socket socket;    ///< As socket capability
};

_Static_assert(sizeof(union s3k_cap) == 8, "sizeof(union s3k_cap) != 8");

/**
 * @defgroup api-syscall System Calls
 *
 * System calls for user processes.
 *
 * @{
 */

/// S3K Syscall Numbers
typedef enum s3k_syscall {
	// Capabilityless syscalls
	S3K_SYSCALL_GETPID,  ///< Get process ID
	S3K_SYSCALL_GETREG,  ///< Get register value
	S3K_SYSCALL_SETREG,  ///< Set register value
	S3K_SYSCALL_YIELD,   ///< Yield remaining time slice
	// Capability syscalls
	S3K_SYSCALL_GETCAP,  ///< Get capability description
	S3K_SYSCALL_MOVCAP,  ///< Move capability
	S3K_SYSCALL_DELCAP,  ///< Delete capability
	S3K_SYSCALL_REVCAP,  ///< Revoke capability
	S3K_SYSCALL_DRVCAP,  ///< Derive capability
	// Monitor syscalls
	S3K_SYSCALL_MSUSPEND,  ///< Monitor suspend process
	S3K_SYSCALL_MRESUME,   ///< Monitor resume process
	S3K_SYSCALL_MGETREG,   ///< Monitor get register value
	S3K_SYSCALL_MSETREG,   ///< Monitor set register value
	S3K_SYSCALL_MGETCAP,   ///< Monitor get capability description
	S3K_SYSCALL_MTAKECAP,  ///< Monitor take capability
	S3K_SYSCALL_MGIVECAP,  ///< Monitor give capability
	// IPC syscalls
	S3K_SYSCALL_RECV,      ///< Receive message/capability
	S3K_SYSCALL_SEND,      ///< Send message/capability
	S3K_SYSCALL_SENDRECV,  ///< Send then receive message/capability
} s3k_syscall_t;

/**
 * @brief Get the process ID.
 *
 * @return Process ID.
 */
uint64_t s3k_getpid(void);

/**
 * @brief Get the value of a register.
 *
 * @param reg Register ID.
 * @return Read value.
 * @warning Register IDs that are out-of-range are silently ignored.
 */
uint64_t s3k_getreg(enum s3k_reg reg);

/**
 * @brief Read and set value.
 *
 * @param reg Register ID.
 * @param val Value to write.
 * @return Read value.
 * @warning Register IDs that are out-of-range are silently ignored.
 */
uint64_t s3k_setreg(enum s3k_reg reg, uint64_t val);

/**
 * @brief Yield remaining time slice.
 */
void s3k_yield(void);

/**
 * @brief Reads a capability description.
 *
 * @param i Index of capability table.
 * @return The read capability.
 */
union s3k_cap s3k_getcap(uint64_t i);

/**
 * @brief Moves a capability.
 *
 * @param i Index of the capability to move.
 * @param j Destination index.
 * @return `S3K_EXCPT_INDEX` if index i or j is out-of-range.
 * @return `S3K_EXCPT_EMPTY` if slot i is empty.
 * @return `S3K_EXCPT_NONE` if move was successful.
 */
enum s3k_excpt s3k_movcap(uint64_t i, uint64_t j);

/**
 * @brief Deletes a capability.
 *
 * @param i Index of the capability to delete.
 * @return `S3K_EXCPT_INDEX` if index i is out-of-range.
 * @return `S3K_EXCPT_EMPTY` if slot i is empty.
 * @return `S3K_EXCPT_NONE` if deletion was successful.
 */
enum s3k_excpt s3k_delcap(uint64_t i);

/**
 * @brief Revokes a capability.
 *
 * Deletes all of of the capabilities descendants.
 *
 * @param i Index of the capability to revoke.
 * @return `S3K_EXCPT_INDEX` if index i is out-of-range.
 * @return `S3K_EXCPT_EMPTY` if slot i is empty.
 * @return `S3K_EXCPT_PREEMPTED` if the system call is preempted.
 * @return `S3K_EXCPT_NONE` if revokation was successful.
 * @warning If preempted, only a subset of the descendants will be deleted.
 */
enum s3k_excpt s3k_revcap(uint64_t i);

/**
 * @brief Derives a new capability.
 *
 * Creates a new capability based on another. Capability derivation will fail if the corresponding
 * `s3k_cap_deriveable` call returns false.
 *
 * @param i Index of the capability to revoke.
 * @param j Destination index.
 * @param cap Description of the derived capability.
 * @return `S3K_EXCPT_INDEX` if index i or j is out-of-range.
 * @return `S3K_EXCPT_EMPTY` if slot i is empty.
 * @return `S3K_EXCPT_OCCUPIED` if slot j is occupied.
 * @return `S3K_EXCPT_DERIVATION` if slot cap can **not** be derived from the capability in index i.
 * @return `S3K_EXCPT_NONE` if capability was successfully derived.
 */
enum s3k_excpt s3k_drvcap(uint64_t i, uint64_t j, union s3k_cap cap);

enum s3k_excpt s3kReceive(void);
enum s3k_excpt s3kSend(void);
enum s3k_excpt s3kSendReceive(void);
/**
 * @brief Monitor suspends a process.
 *
 * @param i Index to monitor capability.
 * @param pid Process to suspend.
 * @return TODO
 */
enum s3k_excpt s3k_msuspend(uint64_t i, uint64_t pid);
/**
 * @brief Monitor resumes a process.
 *
 * @param i Index to monitor capability.
 * @param pid Process to resume.
 * @return TODO
 */
enum s3k_excpt s3k_mresume(uint64_t i, uint64_t pid);
/**
 * @brief Monitor reads a process's register.
 *
 * @param i Index to monitor capability.
 * @param pid Process...
 * @param reg Register to read
 * @param val Read value
 * @return TODO
 */
enum s3k_excpt s3k_mgetreg(uint64_t i, uint64_t pid, enum s3k_reg reg, uint64_t *val);
/**
 * @brief Monitor write a process's register.
 *
 * @param i Index to monitor capability.
 * @param pid Process...
 * @param reg Register to read
 * @param val Value to write
 * @return TODO
 */
enum s3k_excpt s3k_msetreg(uint64_t i, uint64_t pid, enum s3k_reg reg, uint64_t val);
/**
 * @brief Monitor read process's capability.
 *
 * @param i Index to monitor capability.
 * @param pid Process to read capability of.
 * @param j Index of capability to read
 * @param cap Read capability
 * @return TODO
 */
enum s3k_excpt s3k_mgetcap(uint64_t i, uint64_t pid, uint64_t j, union s3k_cap *cap);
/**
 * @brief Monitor give a capability to a process.
 *
 * @param i Index to monitor capability.
 * @param pid Process to give capability.
 * @param src Index of capability to give
 * @param dst Destination of capability
 * @return TODO
 */
enum s3k_excpt s3k_mgivecap(uint64_t i, uint64_t pid, uint64_t src, uint64_t dst);
/**
 * @brief Monitor take a capability from a process.
 *
 * @param i Index to monitor capability.
 * @param pid Process to take capability from.
 * @param src Index of capability to take
 * @param dst Destination of capability
 * @return TODO
 */
enum s3k_excpt s3k_mtakecap(uint64_t i, uint64_t pid, uint64_t src, uint64_t dst);
/// @}

/**************************** API UTILITY *********************************/

/**
 * @brief Returns a PMP NAPOT representation of the address range.
 * @param begin Start of the range
 * @param end End of the range
 * @return The PMP NAPOT representation of the range.
 * @warning If there is not NAPOT representation, then returns 0.
 */
uint64_t pmp_napot_addr(uint64_t begin, uint64_t end) __attribute__((const));
/**
 * @brief Returns start of the range of PMP NAPOT address.
 * @param addr The PMP NAPOT address.
 * @return The start of the address range.
 */
uint64_t pmp_napot_begin(uint64_t addr);

/**
 * @brief Returns the end of the range of PMP NAPOT address.
 * @param addr The PMP NAPOT address
 * @return The end of the address range.
 */
uint64_t pmp_napot_end(uint64_t addr);

/**
 * @brief Create a time slice capability
 * @param hartid Hardware thread to run on.
 * @param begin Start of time slice.
 * @param end End of time slice.
 * @return A time slice capability.
 */
union s3k_cap s3k_time(uint64_t hartid, uint64_t begin, uint64_t end);

/**
 * @brief Create a memory slice capability
 *
 * To compress the representation of memory slices, we have split the beginning and end of the
 * memory slice with offset. The start of the memory slice is `(offset << 27) + (begin << 12)', the
 * end is `(offset << 27) + (begin << 12)'.
 *
 * @param begin Start of memory slice.
 * @param end End of end slice.
 * @param offset 128 MiB offset
 * @param rwx Read, write, and execute permissions.
 * @return A memory slice capability.
 */
union s3k_cap s3k_memory(uint64_t begin, uint64_t end, uint64_t offset, uint64_t rwx);

/**
 * @brief Create a PMP NAPOT frame capability
 *
 * @param addr PMP address in NAPOT format.
 * @param cfg PMP cfg.
 * @return A PMP capability.
 */
union s3k_cap s3k_pmp(uint64_t addr, uint64_t rwx);

/**
 * @brief Create a monitor slice capability
 *
 * @param begin Start of monitored processes' PID.
 * @param end End of monitored processes' PID.
 * @return A monitor slice capability.
 */
union s3k_cap s3k_monitor(uint64_t begin, uint64_t end);
/// Create a channel slice capability
union s3k_cap s3k_channel(uint64_t begin, uint64_t end);
/// Create a socket capability
union s3k_cap s3k_socket(uint64_t port, uint64_t tag);
/// Check if a time slice can derive the child.
bool s3k_time_derive(union s3k_cap parent, union s3k_cap child);
/// Check if a memory slice can derive the child.
bool s3k_memory_derive(union s3k_cap parent, union s3k_cap child);
/// Check if a monitor slice can derive the child.
bool s3k_monitor_derive(union s3k_cap parent, union s3k_cap child);
/// Check if channel slice can derive the child.
bool s3k_channel_derive(union s3k_cap parent, union s3k_cap child);
/// Check if socket slice can derive the child.
bool s3k_socket_derive(union s3k_cap parent, union s3k_cap child);
/// Check if parent can derive the child.
bool s3k_can_derive(union s3k_cap parent, union s3k_cap child);
/// Check if the time slice capability is a parent
bool s3k_time_parent(union s3k_cap parent, union s3k_cap child);
/// Check if the memory slice capability is a parent
bool s3k_memory_parent(union s3k_cap parent, union s3k_cap child);
/// Check if the monitor slice capability is a parent
bool s3k_monitor_parent(union s3k_cap parent, union s3k_cap child);
/// Check if the channel slice capability is a parent
bool s3k_channel_parent(union s3k_cap parent, union s3k_cap child);
/// Check if the socket slice capability is a parent
bool s3k_socket_parent(union s3k_cap parent, union s3k_cap child);
/// Check if capability is a parent
bool s3k_is_parent(union s3k_cap parent, union s3k_cap child);

/// @}

#endif /* _S3K_H_ */
