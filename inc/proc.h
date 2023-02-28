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

struct regs {
	uint64_t pc;	 ///< Program counter
	uint64_t ra;	 ///< Return address (GPR)
	uint64_t sp;	 ///< Stack pointer (GPR)
	uint64_t gp;	 ///< Global pointer (GPR)
	uint64_t tp;	 ///< Thread pointer (GPR)
	uint64_t t0;	 ///< Temporary register (GPR)
	uint64_t t1;	 ///< Temporary register (GPR)
	uint64_t t2;	 ///< Temporary register (GPR)
	uint64_t s0;	 ///< Saved register/Stack frame pointer (GPR)
	uint64_t s1;	 ///< Saved register (GPR)
	uint64_t a0;	 ///< Argument/Return register (GPR)
	uint64_t a1;	 ///< Argument/Return register (GPR)
	uint64_t a2;	 ///< Argument register (GPR)
	uint64_t a3;	 ///< Argument register (GPR)
	uint64_t a4;	 ///< Argument register (GPR)
	uint64_t a5;	 ///< Argument register (GPR)
	uint64_t a6;	 ///< Argument register (GPR)
	uint64_t a7;	 ///< Argument register (GPR)
	uint64_t s2;	 ///< Saved register (GPR)
	uint64_t s3;	 ///< Saved register (GPR)
	uint64_t s4;	 ///< Saved register (GPR)
	uint64_t s5;	 ///< Saved register (GPR)
	uint64_t s6;	 ///< Saved register (GPR)
	uint64_t s7;	 ///< Saved register (GPR)
	uint64_t s8;	 ///< Saved register (GPR)
	uint64_t s9;	 ///< Saved register (GPR)
	uint64_t s10;	 ///< Saved register (GPR)
	uint64_t s11;	 ///< Saved register (GPR)
	uint64_t t3;	 ///< Temporary register (GPR)
	uint64_t t4;	 ///< Temporary register (GPR)
	uint64_t t5;	 ///< Temporary register (GPR)
	uint64_t t6;	 ///< Temporary register (GPR)
	uint64_t cause;	 ///< Exception cause code.
	uint64_t tval;	 ///< Exception value.
	uint64_t epc;	 ///< Exception program counter.
	uint64_t tvec;	 ///< Exception handling vector.
	uint64_t pmp;	 ///< PMP configuration.
};

/**
 * @brief Process control block.
 *
 * Contains all information needed manage a process except the capabilities.
 */
struct proc {
	/** The registers of the process (RISC-V registers and virtual registers). */
	struct regs regs;
	/** Process ID. */
	uint64_t pid;
	/** Process state. */
	uint64_t state;
	/** Sleep until. */
	uint64_t sleep;
};

/// An array of all processes control blocks.
extern struct proc processes[NPROC];

/// Thread local pointer to current process.
register struct proc *current __asm__("tp");

/**
 * Loads the PMP settings of the process to the hardware.
 * @param proc Process for which we load PMP settings.
 */
void proc_load_pmp(const struct proc *proc);

#endif /* __PROC_H__ */
