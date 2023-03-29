/**
 * @file proc.h
 * @brief Defines the process control block and its associated functions.
 *
 * This file contains the definition of the `struct proc` data structure, which
 * represents a process control block (PCB) in the operating system. It also
 * contains the declarations of functions for manipulating the PCB.
 *
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */
#ifndef __PROC_H__
#define __PROC_H__

#include "cnode.h"

#include <stdint.h>

enum reg {
	/* General purpose registers */
	REG_PC,	 ///< Program counter
	REG_RA,	 ///< Return address (GPR)
	REG_SP,	 ///< Stack pointer (GPR)
	REG_GP,	 ///< Global pointer (GPR)
	REG_TP,	 ///< Thread pointer (GPR)
	REG_T0,	 ///< Temporary register (GPR)
	REG_T1,	 ///< Temporary register (GPR)
	REG_T2,	 ///< Temporary register (GPR)
	REG_S0,	 ///< Saved register/Stack frame pointer (GPR)
	REG_S1,	 ///< Saved register (GPR)
	REG_A0,	 ///< Argument/Return register (GPR)
	REG_A1,	 ///< Argument/Return register (GPR)
	REG_A2,	 ///< Argument register (GPR)
	REG_A3,	 ///< Argument register (GPR)
	REG_A4,	 ///< Argument register (GPR)
	REG_A5,	 ///< Argument register (GPR)
	REG_A6,	 ///< Argument register (GPR)
	REG_A7,	 ///< Argument register (GPR)
	REG_S2,	 ///< Saved register (GPR)
	REG_S3,	 ///< Saved register (GPR)
	REG_S4,	 ///< Saved register (GPR)
	REG_S5,	 ///< Saved register (GPR)
	REG_S6,	 ///< Saved register (GPR)
	REG_S7,	 ///< Saved register (GPR)
	REG_S8,	 ///< Saved register (GPR)
	REG_S9,	 ///< Saved register (GPR)
	REG_S10, ///< Saved register (GPR)
	REG_S11, ///< Saved register (GPR)
	REG_T3,	 ///< Temporary register (GPR)
	REG_T4,	 ///< Temporary register (GPR)
	REG_T5,	 ///< Temporary register (GPR)
	REG_T6,	 ///< Temporary register (GPR)
	/* Virtual registers */
	/* Trap handling setup */
	REG_TPC, ///< Trap program counter.
	REG_TSP, ///< Trap stack pointer.
	/* Exception handling registers */
	REG_EPC,    ///< Exception program counter.
	REG_ESP,    ///< Exception stack pointer.
	REG_ECAUSE, ///< Exception cause code.
	REG_EVAL,   ///< Exception value.
	/* PMP registers */
	REG_PMP, ///< PMP configuration.
	/* End of registers */
	REG_COUNT ///< *Number of S3K registers.*
};

/**
 * @brief Process control block.
 *
 * Contains all information needed manage a process except the capabilities.
 */
struct proc {
	/** The registers of the process (RISC-V registers and virtual
	 * registers). */
	uint64_t regs[REG_COUNT];
	/** Process ID. */
	uint64_t pid;
	/** Process state. */
	uint64_t state;
	/** Sleep until. */
	uint64_t sleep;
	/** Capability destination for receive calls */
	uint64_t cap_dest;
};

/**
 * Initializes all processes in the system.
 *
 * @param payload A pointer to the boot loader's code.
 *
 * @note This function should be called only once during system startup.
 */
void proc_init(uint64_t payload);

/**
 * @brief Gets the process corresponding to a given process ID.
 *
 * @param pid The process ID to look for.
 * @return A pointer to the process corresponding to the given PID.
 */
struct proc *proc_get(uint64_t pid);

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
bool proc_acquire(struct proc *proc, uint64_t expected);

/**
 * @brief Release the lock on a process.
 *
 * The process's lock is embedded in its state. This function sets the LSB of
 * the state to 0 to unlock the process.
 *
 * @param proc Pointer to the process to release the lock for.
 */
void proc_release(struct proc *proc);

/**
 * Set the process to a suspended state without locking it. The process may
 * still be running, but it will not resume after its timeslice has ended.
 *
 * @param proc Pointer to process to suspend.
 */
void proc_suspend(struct proc *proc);

/**
 * Resumes a process from its suspend state without locking it.
 *
 * @param proc Pointer to process to be resumed.
 */
void proc_resume(struct proc *proc);

/**
 * The process is set to wait on a channel atomically if it is not ordered to
 * suspend. After begin set to wait, schedule_next() should be called. If
 * ordered to suspend, schedule_yield() should be called.
 */
bool proc_ipc_wait(struct proc *proc, uint64_t channel_id);

/**
 * The process is waiting for an IPC send, the channel it is waiting on is
 * included in its state. This function will atomically acquire the process
 * if its state is waiting on the provided channel id. The processes is
 * released with proc_ipc_release().
 */
bool proc_ipc_acquire(struct proc *proc, uint64_t channel_id);

/**
 * @brief Loads the PMP settings of the process to the hardware.
 *
 * This function loads the PMP settings of the process to the hardware. The PMP
 * settings specify the memory regions that the process can access. This
 * function loads the PMP settings to the hardware so that the hardware enforces
 * the process's memory access permissions.
 *
 * @param proc Pointer to the process for which we load PMP settings.
 */
void proc_load_pmp(const struct proc *proc);

#endif /* __PROC_H__ */
