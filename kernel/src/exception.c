/* See LICENSE file for copyright and license details. */
#include "exception.h"

#include "csr.h"
#include "kernel.h"
#include "proc.h"
#include "trap.h"

#define ILLEGAL_INSTRUCTION 0x2

#define MRET 0x30200073
#define SRET 0x10200073
#define URET 0x00200073

proc_t *handle_exception(void)
{
	kprintf(1, "> handle_exception(pid=%X,mcause=%X,mtval=%X,mepc=%X)\n",
		current->pid, csrr(mcause), csrr(mtval), csrr(mepc));
	/* Check if it is a return from exception */
	current->regs[REG_ECAUSE] = csrr(mcause);
	current->regs[REG_EVAL] = csrr(mtval);
	current->regs[REG_EPC] = current->regs[REG_PC];
	current->regs[REG_ESP] = current->regs[REG_SP];
	current->regs[REG_PC] = current->regs[REG_TPC];
	current->regs[REG_SP] = current->regs[REG_TSP];
	if (!current->regs[REG_PC]) {
		proc_suspend(current);
		return NULL;
	}
	return current;
}

static proc_t *handle_trap_return(void)
{
	kprintf(1, "> handle_trap_return(pid=%X)\n", current->pid);
	current->regs[REG_PC] = current->regs[REG_EPC];
	current->regs[REG_SP] = current->regs[REG_ESP];
	current->regs[REG_ECAUSE] = 0;
	current->regs[REG_EVAL] = 0;
	current->regs[REG_EPC] = 0;
	current->regs[REG_ESP] = 0;
	return current;
}

proc_t *handle_illegal_instruction(void)
{
	switch (csrr(mtval)) {
	case MRET:
	case SRET:
	case URET:
		return handle_trap_return();
	default:
		return handle_exception();
	}
}
