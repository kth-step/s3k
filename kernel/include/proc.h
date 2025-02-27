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

#include "cap/table.h"
#include "cap/types.h"

#include <stdbool.h>
#include <stdint.h>

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
	/* Special value for number of registers */
	REG_CNT,
} reg_t;

/**
 * @brief Process control block.
 *
 * Contains all information needed manage a process except the capabilities.
 */
typedef struct {
	/** Process state. */
	proc_state_t state;
	/** The registers of the process (RISC-V registers and virtual
     * registers). */
	uint64_t regs[REG_CNT];
	/** PMP registers */
	uint8_t pmpcfg[NPMP];
	uint64_t pmpaddr[NPMP];
	/** Instrumentation registers */
	/** Process ID. */
	pid_t pid;

	/** Scheduling information */
	uint64_t timeout;
	/**
     * Minimum remaining time required for receiving messages.
     * If a client does not have sufficient execution time,
     * it is not allowed to send the message.
     */
	uint64_t serv_time;
} proc_t;

/**
 * Initializes all processes in the system.
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

proc_state_t proc_get_state(proc_t *proc);

bool proc_acquire(proc_t *proc);
void proc_release(proc_t *proc);
void proc_suspend(proc_t *proc);
void proc_resume(proc_t *proc);
void proc_ipc_wait(proc_t *proc, chan_t chan);
bool proc_ipc_acquire(proc_t *proc, chan_t chan);
bool proc_is_suspended(proc_t *proc);

bool proc_pmp_avail(proc_t *proc, pmp_slot_t slot);
void proc_pmp_load(proc_t *proc, pmp_slot_t slot, rwx_t cfg, napot_t addr);
void proc_pmp_unload(proc_t *proc, pmp_slot_t slot);
void proc_pmp_sync(proc_t *proc);
