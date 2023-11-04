/* See LICENSE file for copyright and license details. */
#include "exception.h"

#include "csr.h"
#include "current.h"
#include "kernel.h"
#include "proc.h"
#include "trap.h"

#define ILLEGAL_INSTRUCTION 0x2

#define MRET 0x30200073
#define SRET 0x10200073
#define URET 0x00200073

void handle_exception(void)
{
	/* Check if it is a return from exception */
	kernel_preempt_disable();
	current->regs[REG_ECAUSE] = csrr_mcause();
	current->regs[REG_EVAL] = csrr_mtval();
	current->regs[REG_EPC] = current->regs[REG_PC];
	current->regs[REG_ESP] = current->regs[REG_SP];
	current->regs[REG_PC] = current->regs[REG_TPC];
	current->regs[REG_SP] = current->regs[REG_TSP];
	kernel_preempt_enable();
}

static void handle_trap_return(void)
{
	kernel_preempt_disable();
	current->regs[REG_PC] = current->regs[REG_EPC];
	current->regs[REG_SP] = current->regs[REG_ESP];
	current->regs[REG_ECAUSE] = 0;
	current->regs[REG_EVAL] = 0;
	current->regs[REG_EPC] = 0;
	current->regs[REG_ESP] = 0;
	kernel_preempt_enable();
}

void handle_illegal_instruction(void)
{
	switch (csrr_mtval()) {
	case MRET:
	case SRET:
	case URET:
		handle_trap_return();
		break;
	default:
		handle_exception();
		break;
	}
}
