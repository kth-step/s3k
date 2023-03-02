/**
 * @file proc.h
 * @brief Process control block.
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */
#ifndef __PROC_H__
#define __PROC_H__

#include <stdint.h>

#include "cnode.h"
#include "lock.h"

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
};

/// An array of all processes control blocks.
extern struct proc processes[NPROC];

/**
 * Loads the PMP settings of the process to the hardware.
 * @param proc Process for which we load PMP settings.
 */
void proc_load_pmp(const struct proc *proc);

#endif /* __PROC_H__ */
