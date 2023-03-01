/* See LICENSE file for copyright and license details. */
#include "exception.h"

#include "proc.h"
#include "trap.h"

void handle_exception(uint64_t mcause, uint64_t mepc, uint64_t mtval)
{
	current->regs[REG_ECAUSE] = mcause;
	current->regs[REG_EVAL] = mtval;
	current->regs[REG_EPC] = current->regs[REG_PC];
	current->regs[REG_ESP] = current->regs[REG_SP];
	current->regs[REG_PC] = current->regs[REG_TPC];
	current->regs[REG_SP] = current->regs[REG_TSP];
}
