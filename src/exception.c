/* See LICENSE file for copyright and license details. */
#include "exception.h"

#include "proc.h"
#include "trap.h"

#define ILLEGAL_INSTRUCTION 0x2

#define MRET 0x30200073

static void handle_ret(void) {
	current->regs[REG_PC] = current->regs[REG_EPC];
	current->regs[REG_SP] = current->regs[REG_ESP];
	current->regs[REG_ECAUSE] = 0;
	current->regs[REG_EVAL] = 0;
	current->regs[REG_EPC] = 0;
	current->regs[REG_ESP] = 0;
}

static void handle_default(uint64_t mcause, uint64_t mepc, uint64_t mtval) {
	current->regs[REG_ECAUSE] = mcause;
	current->regs[REG_EVAL] = mtval;
	current->regs[REG_EPC] = current->regs[REG_PC];
	current->regs[REG_ESP] = current->regs[REG_SP];
	current->regs[REG_PC] = current->regs[REG_TPC];
	current->regs[REG_SP] = current->regs[REG_TSP];
}

void handle_exception(uint64_t mcause, uint64_t mepc, uint64_t mtval)
{
	if (mcause == ILLEGAL_INSTRUCTION && mtval == MRET) {
		handle_ret()
	} else {
		handle_default();
	}
}

