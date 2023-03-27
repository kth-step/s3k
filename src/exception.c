/* See LICENSE file for copyright and license details. */
#include "exception.h"

#include "proc.h"
#include "trap.h"

#define ILLEGAL_INSTRUCTION 0x2

#define MRET 0x30200073
#define SRET 0x10200073
#define URET 0x00200073

/**
 * This function restores the program counter and stack pointer to their values
 * prior to the exception, and clears the exception cause and exception value
 * registers.
 */
static void handle_ret(struct proc *proc)
{
	proc->regs[REG_PC] = proc->regs[REG_EPC];
	proc->regs[REG_SP] = proc->regs[REG_ESP];
	proc->regs[REG_ECAUSE] = 0;
	proc->regs[REG_EVAL] = 0;
	proc->regs[REG_EPC] = 0;
	proc->regs[REG_ESP] = 0;
}

/*
 * This function is called when an exception occurs that doesn't fall under the
 * category of an illegal instruction return, such as a page fault or a timer
 * interrupt. It updates the exception cause, value, program counter, and stack
 * pointer in the process's registers, and switches to the trap handler program
 * counter and stack pointer.
 */
static void handle_default(struct proc *proc, uint64_t mcause, uint64_t mepc,
			   uint64_t mtval)
{
	proc->regs[REG_ECAUSE] = mcause;
	proc->regs[REG_EVAL] = mtval;
	proc->regs[REG_EPC] = proc->regs[REG_PC];
	proc->regs[REG_ESP] = proc->regs[REG_SP];
	proc->regs[REG_PC] = proc->regs[REG_TPC];
	proc->regs[REG_SP] = proc->regs[REG_TSP];
}

void handle_exception(struct proc *proc, uint64_t mcause, uint64_t mepc,
		      uint64_t mtval)
{
	/* Check if it is a return from exception */
	if (mcause == ILLEGAL_INSTRUCTION
	    && (mtval == MRET || mtval == SRET || mtval == URET)) {
		// Handle return from exception
		handle_ret(proc);
	} else {
		// Handle default exception
		handle_default(proc, mcause, mepc, mtval);
	}
}
