#pragma once

#include <limits.h>  ///< For limits of integer types.
#include <stdbool.h> ///< For boolean type.
#include <stddef.h>  ///< For size_t definition.
#include <stdint.h>  ///< For standard integer types.

// Machine word-sized integer types.
typedef unsigned long word_t; ///< Unsigned machine word-sized integer.

typedef uint16_t pid_t; ///< Process identifier type.

typedef uint16_t fuel_t; ///< Fuel type for capabilities.

typedef uint16_t time_slot_t; ///< Slot type for scheduling.
typedef uint8_t hart_t;	      ///< Hart ID type for hardware threads.

typedef uint32_t index_t; ///< Index type for various tables.

typedef uint8_t pmp_slot_t; ///< PMP slot type for memory protection.

/**
 * @enum pmp_mode
 * @brief PMP configuration modes.
 */
enum pmp_mode {
	PMP_MODE_OFF = 0x00,   ///< PMP disabled.
	PMP_MODE_TOR = 0x08,   ///< Top of Range mode.
	PMP_MODE_NA4 = 0x10,   ///< Naturally aligned 4-byte region.
	PMP_MODE_NAPOT = 0x18, ///< Naturally aligned power-of-two region.
};

typedef uint8_t pmp_mode_t; ///< PMP mode type.
typedef word_t pmp_addr_t;  ///< PMP address type.
typedef uint8_t pmp_cfg_t;  ///< PMP configuration type.

/**
 * @enum mem_perm
 * @brief Memory permission values.
 */
enum mem_perm {
	MEM_PERM_NONE = 0x0, ///< No permission.
	MEM_PERM_R = 0x1,    ///< Read-only permission.
	MEM_PERM_RW = 0x3,   ///< Read and write permission.
	MEM_PERM_RX = 0x5,   ///< Read and execute permission.
	MEM_PERM_RWX = 0x7,  ///< Read, write, and execute permission.
};

typedef uint8_t mem_perm_t;  ///< Memory permission type.
typedef uint32_t mem_addr_t; ///< Memory address type.

/**
 * @enum err
 * @brief Error codes for system calls.
 *
 * Negative values indicate errors, while zero indicates success.
 */
typedef enum err {
	ERR_SUCCESS = 0,	   ///< Operation successful.
	ERR_INVALID_ACCESS = -1,   ///< Invalid access.
	ERR_INVALID_ARGUMENT = -2, ///< Invalid argument.
	ERR_INVALID_STATE = -3,	   ///< Invalid state.
	ERR_SLOT_IN_USE = -4,	   ///< Slot already in use.
	ERR_TIMEOUT = -5,	   ///< Timeout occurred.
} err_t;

/**
 * @enum ipc_mode
 * @brief IPC modes for inter-process communication.
 *
 * Enumerates the modes of IPC, including synchronous and asynchronous options.
 */
enum ipc_mode {
	IPC_MODE_NONE = 0,   ///< No IPC mode.
	IPC_MODE_USYNC = 1,  ///< Unidirectional synchronous IPC.
	IPC_MODE_BSYNC = 2,  ///< Bidirectional synchronous IPC.
	IPC_MODE_ASYNC = 3,  ///< Asynchronous IPC.
	IPC_MODE_MASK = 0x3, ///< Mask for IPC modes.
	IPC_MODE_REVOKE = 0x4, ///< Revoke flag for IPC.
};

typedef uint8_t ipc_mode_t;

enum ipc_flag {
	IPC_FLAG_YIELD = 1,   ///< Yield flag for IPC.
	IPC_FLAG_TSL = 2,     ///< Permission to send time slice capability.
	IPC_FLAG_MEM = 4,     ///< Permission to send memory capability.
	IPC_FLAG_MON = 8,     ///< Permission to send monitor capability.
	IPC_FLAG_IPC = 16,    ///< Permission to send IPC capability.
	IPC_FLAG_MASK = 0x1F, ///< Mask for IPC flags.
};

typedef uint8_t ipc_flag_t;

/**
 * @enum capty
 * @brief Capability types.
 *
 * Enumerates the types of capabilities available in the system.
 * Used in IPC capabilities to identify the type of capability being sent or received.
 */
typedef enum capty {
	CAPTY_NONE = 0x0, ///< No capability.
	CAPTY_MEM = 0x1,  ///< Memory capability.
	CAPTY_TSL = 0x2,  ///< Time slice capability.
	CAPTY_MON = 0x3,  ///< Monitor capability.
	CAPTY_IPC = 0x4,  ///< IPC capability.
} capty_t;

#define INVALID_PID ((pid_t)0)					       ///< Invalid PID constant.
#define MAX_PID ((pid_t)_MAX_PID)				       ///< Maximum PID constant.
#define MAX_PMP_SLOT ((pmp_slot_t)_MAX_PMP_SLOT)		       ///< Maximum PMP slot constant.
#define MAX_MEMORY_FUEL ((fuel_t)_MAX_MEMORY_FUEL)		       ///< Maximum memory capabilities.
#define MEM_TABLE_SIZE ((index_t)(MAX_MEMORY_FUEL * _NUM_MEMORY_CAPS)) ///< Maximum memory index.
#define NUM_MEMORY_CAPS ((index_t)_NUM_MEMORY_CAPS)		       ///< Number of memory capability slots.
#define MAX_TIME_FUEL ((fuel_t)_MAX_TIME_FUEL)			       ///< Maximum time capabilities.
#define TSL_TABLE_SIZE ((index_t)(_MAX_TIME_FUEL * _NUM_HARTS))	       ///< Maximum time index.
#define MAX_MONITOR_FUEL ((fuel_t)_MAX_MONITOR_FUEL)		       ///< Maximum monitor capabilities.
#define MON_TABLE_SIZE ((index_t)(MAX_MONITOR_FUEL * MAX_PID))	       ///< Maximum monitor index.
#define MAX_TIME_SLOT ((time_slot_t)_MAX_TIME_SLOT)		       ///< Maximum time slot constant.
#define MAX_IPC_FUEL ((fuel_t)_MAX_IPC_FUEL)			       ///< Maximum IPC capabilities.
#define IPC_TABLE_SIZE ((index_t)(MAX_IPC_FUEL))		       ///< Maximum IPC index.
#define NUM_HARTS ((hart_t)_NUM_HARTS)				       ///< Number of harts constant.
#define SMP ((hart_t)_NUM_HARTS > 1)				       ///< Symmetric multiprocessing check.
