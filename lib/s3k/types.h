#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef unsigned long s3k_word_t; // Unsigned machine word-sized integer.

typedef uint16_t s3k_pid_t; ///< Process identifier type.

typedef uint16_t s3k_fuel_t; ///< Fuel type for capabilities.

typedef uint64_t s3k_time_t;

typedef uint32_t s3k_index_t;	   ///< Index type for capabilities.
typedef uint8_t s3k_hart_t;	   ///< Hart type for capabilities.
typedef uint16_t s3k_time_slot_t;  ///< Time slot type for capabilities.
typedef uint32_t s3k_mem_addr_t;   ///< Memory address type for capabilities.
typedef uint8_t s3k_pmp_slot_t;	   ///< PMP slot type for capabilities.
typedef uint8_t s3k_cap_slot_t;	   ///< Capability slot type for capabilities.
typedef uint8_t s3k_mem_perm_t;	   ///< Memory permission type for capabilities.
typedef uint8_t s3k_ipc_mode_t;   ///< IPC mode type for capabilities.
typedef uint8_t s3k_ipc_flag_t;   ///< IPC flag type for capabilities.
typedef s3k_word_t s3k_pmp_addr_t; ///< PMP address type for capabilities.

/**
 * @enum s3k_err
 * @brief Error codes for system calls.
 *
 * Negative values indicate errors, while zero indicates success.
 */
typedef enum s3k_err {
	S3K_SUCCESS = 0,      ///< Operation successful.
	S3K_ERR_INVARG = -1,  ///< Invalid argument.
	S3K_ERR_INVACC = -2,  ///< Invalid access.
	S3K_ERR_INVIDX = -3,  ///< Invalid index.
	S3K_ERR_INVDRV = -4,  ///< Invalid derivation.
	S3K_ERR_BADCAP = -5,  ///< Bad capability.
	S3K_ERR_SLOTUSE = -6, ///< Slot already in use.
	S3K_ERR_BADADDR = -7, ///< Bad address.
	S3K_ERR_BADPERM = -8, ///< Bad permissions.
} s3k_err_t;

enum s3k_ipc_mode {
	S3K_IPC_MODE_NONE = 0,	///< No IPC mode.
	S3K_IPC_MODE_USYNC = 1, ///< Unidirectional synchronous IPC mode.
	S3K_IPC_MODE_BSYNC = 2, ///< Bidirectional synchronous IPC mode.
	S3K_IPC_MODE_ASYNC = 3, ///< Asynchronous IPC mode.
	S3K_IPC_MODE_REVOKE = 4,///< Revoke flag for IPC.
};

enum s3k_ipc_flag {
	S3K_IPC_FLAG_YIELD = 1, ///< Yield flag for IPC.
	S3K_IPC_FLAG_TSL = 2,	///< Permission to send time slice capability.
	S3K_IPC_FLAG_MEM = 4,	///< Permission to send memory capability.
	S3K_IPC_FLAG_MON = 8,	///< Permission to send monitor capability.
	S3K_IPC_FLAG_IPC = 16,	///< Permission to send IPC capability.
};

typedef enum s3k_vreg {
	S3K_VREG_TPC = 0,    ///< Trap Program Counter register.
	S3K_VREG_TSP = 1,    ///< Trap Stack Pointer register.
	S3K_VREG_ECAUSE = 2, ///< Exception Cause register.
	S3K_VREG_EVAL = 3,   ///< Exception Value register.
	S3K_VREG_EPC = 4,    ///< Exception Program Counter register.
	S3K_VREG_ESP = 5,    ///< Exception Stack Pointer register.
} s3k_vreg_t;

/**
 * @enum s3k_mem_perm
 * @brief Memory permission values.
 *
 * These values represent the permissions for memory capabilities.
 * - Read-only (R)
 * - Read-write (RW)
 * - Read-execute (RX)
 * - Read-write-execute (RWX)
 */
enum mem_perm {
	S3K_MEM_PERM_NONE = 0x0,  ///< No permission.
	S3K_MEM_PERM_R = 0x1,	///< Read-only permission.
	S3K_MEM_PERM_RW = 0x3,	///< Read-write permission.
	S3K_MEM_PERM_RX = 0x5,	///< Read-execute permission.
	S3K_MEM_PERM_RWX = 0x7, ///< Read-write-execute permission.
};

typedef enum s3k_capty {
	S3K_CAPTY_NONE = 0, ///< No capability type.
	S3K_CAPTY_MEM = 1,  ///< Memory capability type.
	S3K_CAPTY_TSL = 2,  ///< Time SLICE capability type.
	S3K_CAPTY_MON = 3,  ///< Monitor capability type.
	S3K_CAPTY_IPC = 4,  ///< IPC capability type.
} s3k_capty_t;

/**
 * @enum s3k_reg
 * @brief Register identifiers.
 *
 * These values represent the registers used in the S3K architecture.
 * The registers are used for various purposes, including program counter,
 * return address, stack pointer, and general-purpose registers.
 */
typedef enum s3k_reg_t {
	S3K_REG_PC,  ///< Program Counter register.
	S3K_REG_RA,  ///< Return Address register.
	S3K_REG_SP,  ///< Stack Pointer register.
	S3K_REG_GP,  ///< Global Pointer register.
	S3K_REG_TP,  ///< Thread Pointer register.
	S3K_REG_A0,  ///< Argument register 0.
	S3K_REG_A1,  ///< Argument register 1.
	S3K_REG_A2,  ///< Argument register 2.
	S3K_REG_A3,  ///< Argument register 3.
	S3K_REG_A4,  ///< Argument register 4.
	S3K_REG_A5,  ///< Argument register 5.
	S3K_REG_A6,  ///< Argument register 6.
	S3K_REG_A7,  ///< Argument register 7.
	S3K_REG_T0,  ///< Temporary register 0.
	S3K_REG_T1,  ///< Temporary register 1.
	S3K_REG_T2,  ///< Temporary register 2.
	S3K_REG_T3,  ///< Temporary register 3.
	S3K_REG_T4,  ///< Temporary register 4.
	S3K_REG_T5,  ///< Temporary register 5.
	S3K_REG_T6,  ///< Temporary register 6.
	S3K_REG_S0,  ///< Saved register 0.
	S3K_REG_S1,  ///< Saved register 1.
	S3K_REG_S2,  ///< Saved register 2.
	S3K_REG_S3,  ///< Saved register 3.
	S3K_REG_S4,  ///< Saved register 4.
	S3K_REG_S5,  ///< Saved register 5.
	S3K_REG_S6,  ///< Saved register 6.
	S3K_REG_S7,  ///< Saved register 7.
	S3K_REG_S8,  ///< Saved register 8.
	S3K_REG_S9,  ///< Saved register 9.
	S3K_REG_S10, ///< Saved register 10.
	S3K_REG_S11, ///< Saved register 11.
} s3k_reg_t;

/**
 * @struct s3k_cap_memory
 * @brief Memory capability structure.
 *
 * This structure defines a memory capability, which includes information about the owner,
 * the amount of cfree (both remaining and initial), permissions, and the memory region associated with the capability.
 * The cfree acts as a resource or capacity indicator, controlling how many new capabilities can be created
 * and where they will be located in the table.
 */
typedef struct s3k_cap_mem {
	s3k_pid_t owner;      ///< Process ID of the owner of the capability.
	s3k_fuel_t cfree;     ///< Remaining cfree for the capability.
	s3k_fuel_t csize;     ///< Initial cfree allocated to the capability.
	s3k_pmp_slot_t slot;  ///< PMP slot used.
	s3k_mem_perm_t rwx;   ///< Permissions (Read, Write, Execute) encoded as bits.
	s3k_mem_addr_t begin; ///< Start address of the memory region.
	s3k_mem_addr_t end;   ///< End address of the memory region.
} __attribute__((aligned(16))) s3k_cap_mem_t;

/**
 * @struct s3k_cap_time
 * @brief Time capability structure.
 *
 * This structure defines a time capability, which includes information about the owner,
 * the amount of cfree (both remaining and initial), and the time range associated with the capability.
 */
typedef struct s3k_cap_tsl {
	s3k_pid_t owner;       ///< Process ID of the owner of the capability.
	s3k_fuel_t cfree;      ///< Remaining cfree for the capability.
	s3k_fuel_t csize;      ///< Initial cfree allocated to the capability.
	s3k_hart_t hart;       ///< The hart of the time capability.
	bool enabled;	       ///< If the time slots are used.
	s3k_time_slot_t mark;  ///< Start of allocated time slots.
	s3k_time_slot_t begin; ///< Start address of the time slots.
	s3k_time_slot_t end;   ///< End address of the time slots.
} __attribute__((aligned(16))) s3k_cap_tsl_t;

typedef struct s3k_cap_mon {
	s3k_pid_t owner;  ///< Process ID of the owner of the capability.
	s3k_fuel_t cfree; ///< Remaining cfree for the capability.
	s3k_fuel_t csize; ///< Initial cfree allocated to the capability.
	s3k_pid_t pid;	  ///< Process ID of the monitored process.
} __attribute__((aligned(8))) s3k_cap_mon_t;

typedef struct s3k_cap_ipc {
	s3k_pid_t owner;  ///< Process ID of the owner of the capability.
	s3k_fuel_t cfree; ///< Remaining cfree for the capability.
	s3k_fuel_t csize; ///< Initial cfree allocated to the capability.
	s3k_ipc_mode_t mode;
	s3k_ipc_flag_t flag;
	s3k_index_t sink;
	s3k_index_t source;
} __attribute__((aligned(16))) s3k_cap_ipc_t;

_Static_assert(sizeof(s3k_cap_mem_t) == 16, "Memory capability has the wrong size.");
_Static_assert(sizeof(s3k_cap_tsl_t) == 16, "Time capability has the wrong size.");
_Static_assert(sizeof(s3k_cap_mon_t) == 8, "Monitor capability has the wrong size.");
_Static_assert(sizeof(s3k_cap_ipc_t) == 16, "IPC capability has the wrong size.");
