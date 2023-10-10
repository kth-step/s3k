#pragma once
/**
 * @file proc.h
 * @brief Defines the process control block and its associated functions.
 *
 * This file contains the definition of the `proc_t` data structure, which
 * represents a process control block (PCB) in the operating system. It also
 * contains the declarations of functions for manipulating the PCB.
 *
 * @copyright MIT License
 */

#include "cap_table.h"
#include "cap_types.h"
#include "mcslock.h"

#include <stdbool.h>
#include <stdint.h>

/** Process state flags
 * PSF_BUSY: Process has been acquired.
 * PSF_BLOCKED: Waiting for IPC.
 * PSF_SUSPENDED: Waiting for monitor
 */
typedef uint64_t proc_state_t;

typedef enum {
	PSF_BUSY = 1,
	PSF_BLOCKED = 2,
	PSF_SUSPENDED = 4,
} proc_state_flag_t;

typedef enum {
	REG_PC,
	REG_RA,
	REG_SP,
	REG_GP,
	REG_TP,
	REG_T0,
	REG_T1,
	REG_T2,
	REG_S0,
	REG_S1,
	REG_A0,
	REG_A1,
	REG_A2,
	REG_A3,
	REG_A4,
	REG_A5,
	REG_A6,
	REG_A7,
	REG_S2,
	REG_S3,
	REG_S4,
	REG_S5,
	REG_S6,
	REG_S7,
	REG_S8,
	REG_S9,
	REG_S10,
	REG_S11,
	REG_T3,
	REG_T4,
	REG_T5,
	REG_T6,
	REG_TPC,
	REG_TSP,
	REG_EPC,
	REG_ESP,
	REG_ECAUSE,
	REG_EVAL,
	REG_SERVTIME,
	REG_WCET,
	/* Special value for number of registers */
	REG_CNT,
} reg_t;

/**
 * @brief Process control block.
 *
 * Contains all information needed manage a process except the capabilities.
 */
typedef struct {
	/** The registers of the process (RISC-V registers and virtual
	 * registers). */
	uint64_t regs[REG_CNT];
	/** PMP registers */
	uint8_t pmpcfg[S3K_PMP_CNT];
	uint64_t pmpaddr[S3K_PMP_CNT];
	/** Instrumentation registers */
	/** Process ID. */
	pid_t pid;
	/** Process state. */
	proc_state_t state;
	qnode_t qnode;

	/** Scheduling information */

	/***** IPC related things *****/
	/**
	 * Timeout of IPC yielding send.
	 * Time before sender gives up.
	 */
	uint64_t timeout;
	/**
	 * Minimum remaining time required for receiving messages.
	 * If a client does not have sufficient execution time,
	 * it is not allowed to send the message.
	 */
	uint64_t serv_time;
	/**
	 * Source and destination pointer for transmitting capabilities.
	 */
	cte_t cap_buf;
} proc_t;

/**
 * Initializes all processes in the system.
 *
 * @param payload A pointer to the boot loader's code.
 *
 * @note This function should be called only once during system startup.
 */
void proc_init(void);

/**
 * @brief Gets the process corresponding to a given process ID.
 *
 * @param pid The process ID to look for.
 * @return A pointer to the process corresponding to the given PID.
 */
proc_t *proc_get(pid_t pid);

/**
 * @brief Attempt to acquire the lock for a process.
 *
 * The process's lock is embedded in its state. This function attempts to
 * acquire the lock by atomically setting the LSB of the state to 1 if it
 * currently has the value 'expected'. If the lock is already held by another
 * process, this function will return false.
 *
 * @param proc Pointer to the process to acquire the lock for.
 * @param expected The expected value of the process's state.
 * @return True if the lock was successfully acquired, false otherwise.
 */
bool proc_acquire(proc_t *proc);

/**
 * @brief Release the lock on a process.
 *
 * The process's lock is embedded in its state. This function sets the LSB of
 * the state to 0 to unlock the process.
 *
 * @param proc Pointer to the process to release the lock for.
 */
void proc_release(proc_t *proc);

/**
 * Set the process to a suspended state without locking it. The process may
 * still be running, but it will not resume after its timeslice has ended.
 *
 * @param proc Pointer to process to suspend.
 */
void proc_suspend(proc_t *proc);

/**
 * Resumes a process from its suspend state without locking it.
 *
 * @param proc Pointer to process to be resumed.
 */
void proc_resume(proc_t *proc);

void proc_ipc_wait(proc_t *proc, chan_t channel);
bool proc_ipc_acquire(proc_t *proc, chan_t channel);

bool proc_is_suspended(proc_t *proc);

bool proc_pmp_avail(proc_t *proc, pmp_slot_t slot);
void proc_pmp_load(proc_t *proc, pmp_slot_t slot, rwx_t cfg, napot_t addr);
void proc_pmp_unload(proc_t *proc, pmp_slot_t slot);
void proc_pmp_sync(proc_t *proc);
