#pragma once

#include "types.h"

/**
 * @struct proc
 * @brief Process control block (PCB) structure.
 *
 * This structure represents a process in the Svalinn kernel, including its
 * registers, PMP configuration, and process ID.
 */
typedef struct proc {
	word_t state; ///< Process state.

	struct {
		word_t pc, ra, sp, gp, tp;				 ///< Special registers.
		word_t a0, a1, a2, a3, a4, a5, a6, a7;			 ///< Argument registers.
		word_t t0, t1, t2, t3, t4, t5, t6;			 ///< Temporary registers.
		word_t s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11; ///< Saved registers.
	} regs;

	struct {
		pmp_addr_t addr[8]; ///< PMP address for each slot.
		pmp_cfg_t cfg[8];   ///< PMP configuration for each slot.
	} pmp;			    ///< PMP configuration for the process.

	struct {
		word_t tpc, tsp;
		word_t ecause, eval;
		word_t epc, esp;
	} trap;

	uint64_t timeout; ///< Timeout for the process, used for scheduling.
	word_t pid;	  ///< Process ID.
} __attribute__((aligned(sizeof(word_t)))) proc_t;

typedef enum {
	VREG_TPC = 0,
	VREG_TSP = 1,
	VREG_ECAUSE = 2,
	VREG_EVAL = 3,
	VREG_EPC = 4,
	VREG_ESP = 5,
} vreg_t;

/**
 * @enum proc_state
 * @brief Process states.
 *
 * This enumeration defines flags for the different states a process can be in.
 * The states include ready, running, blocked, and suspended.
 */
typedef enum proc_state {
	PROC_STATE_READY = 0,	  ///< Process is ready to run.
	PROC_STATE_ACQUIRED = 1,  /////< Process is acquired and running.
	PROC_STATE_BLOCKED = 2,	  ///< Process is blocked and waiting for an event.
	PROC_STATE_SUSPENDED = 4, ///< Process is suspended and not eligible to run.
} proc_state_t;

/**
 * @brief Check if a PID is valid.
 */
static inline bool proc_valid_pid(pid_t pid)
{
	return pid != INVALID_PID && pid <= MAX_PID;
}

/**
 * @brief Initialize the process table.
 *
 * This function initializes the process table with default values and sets
 * the program counter (PC) of the first process to the given initialization value.
 *
 * @param init The initial program counter value for the first process.
 */
void proc_init(word_t init);

/**
 * @brief Retrieve a process by its PID.
 *
 * This function retrieves a pointer to the process control block (PCB) of the
 * process with the specified PID.
 *
 * @param pid The process ID of the process to retrieve.
 * @return A pointer to the PCB of the specified process.
 */
proc_t *proc_get(pid_t pid);

/**
 * @brief Retrieve the PID of a process.
 *
 * This function calculates and returns the PID of the given process
 * based on its position in the process table.
 *
 * @param p A pointer to the process control block (PCB) of the process.
 * @return The PID of the process.
 * @note The function asserts that the process pointer is valid and within bounds.
 */
pid_t proc_get_pid(const proc_t *p);

/**
 * @brief Set a PMP slot for a process.
 *
 * This function configures a PMP slot for the specified process with the given
 * permissions and address.
 *
 * @param pid The process ID of the process to configure.
 * @param slot The PMP slot to configure.
 * @param rwx The permissions to set (read, write, execute).
 * @param addr The address to associate with the PMP slot.
 */
void proc_pmp_set(pid_t pid, pmp_slot_t slot, mem_perm_t rwx, pmp_addr_t addr);

/**
 * @brief Clear a PMP slot for a process.
 *
 * This function clears the configuration of a PMP slot for the specified process.
 *
 * @param pid The process ID of the process to configure.
 * @param slot The PMP slot to clear.
 */
void proc_pmp_clear(pid_t pid, pmp_slot_t slot);

/**
 * @brief Check if a PMP slot is set for a process.
 *
 * This function checks whether a PMP slot is configured for the specified process.
 *
 * @param pid The process ID of the process to check.
 * @param slot The PMP slot to check.
 * @return `true` if the PMP slot is set, `false` otherwise.
 */
bool proc_pmp_is_set(pid_t pid, pmp_slot_t slot);

/**
 * @brief Get the PMP configuration for a process.
 *
 * This function retrieves the PMP configuration for the specified process.
 *
 * @param pid The process ID of the process to retrieve.
 * @param slot The PMP slot to retrieve.
 * @param rwx A pointer to store the permissions for the PMP slot.
 * @param addr A pointer to store the address for the PMP slot.
 */
void proc_pmp_get(pid_t pid, pmp_slot_t slot, mem_perm_t *rwx, pmp_addr_t *addr);

/**
 * @brief Acquire a process.
 *
 * This function acquires a process with the specified PID, preventing others from running it.
 *
 * @param pid The process ID of the process to acquire.
 * @return `true` if the process was successfully acquired, `false` otherwise.
 * @note This function should be called before executing a process to ensure exclusive access.
 * @note The function asserts that the PID is valid and within bounds.
 */
bool proc_acquire(pid_t pid);

/**
 * @brief Suspend a process.
 *
 * This function suspends the execution of a process with the specified PID.
 *
 * @param pid The process ID of the process to suspend.
 */
void proc_suspend(pid_t pid);

/**
 * @brief Resume a suspended process.
 *
 * This function resumes the execution of a suspended process with the specified PID.
 *
 * @param pid The process ID of the process to resume.
 */
void proc_resume(pid_t pid);

bool proc_ipc_acquire(pid_t pid, index_t i);
bool proc_ipc_block(pid_t pid, index_t i);
void proc_release(pid_t pid);
