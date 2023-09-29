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

#include "types.h"

#include <stdbool.h>
#include <stdint.h>

/** Process state flags
 * PSF_BUSY: Process has been acquired.
 * PSF_BLOCKED: Waiting for IPC.
 * PSF_SUSPENDED: Waiting for monitor
 */
#define PSF_BUSY 1
#define PSF_BLOCKED 2
#define PSF_SUSPENDED 4

#define N_REG (sizeof(trap_frame_t) / sizeof(uint64_t))

typedef struct {
	reg_t pc, ra, sp, gp, tp;
	reg_t t0, t1, t2;
	reg_t s0, s1;
	reg_t a0, a1, a2, a3, a4, a5, a6, a7;
	reg_t s2, s3, s4, s5, s6, s7, s8, s9, s10, s11;
	reg_t t3, t4, t5, t6;
	reg_t tpc, tsp;
	reg_t epc, esp, ecause, eval;
	reg_t preempt;
} trap_frame_t;

/**
 * @brief Process control block.
 *
 * Contains all information needed manage a process except the capabilities.
 */
typedef struct {
	/** The registers of the process (RISC-V registers and virtual
	 * registers). */
	trap_frame_t tf;
	/** PMP registers */
	uint8_t pmpcfg[S3K_PMP_CNT];
	uint64_t pmpaddr[S3K_PMP_CNT];
	/** Instrumentation registers */
	uint64_t instrument_wcet;
	/** Process ID. */
	uint64_t pid;
	/** Process state. */
	uint64_t state;
	/** Sleep until. */
	uint64_t sleep;
	/** timeout */
	uint64_t timeout;
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
proc_t *proc_get(uint64_t pid);

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

void proc_ipc_wait(proc_t *proc, uint64_t channel);
bool proc_ipc_acquire(proc_t *proc, uint64_t channel);

bool proc_is_suspended(proc_t *proc);

bool proc_pmp_avail(proc_t *proc, uint64_t slot);
void proc_pmp_load(proc_t *proc, uint64_t slot, uint64_t cfg, uint64_t addr);
void proc_pmp_unload(proc_t *proc, uint64_t slot);
void proc_pmp_sync(proc_t *proc);
