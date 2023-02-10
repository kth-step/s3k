/* See LICENSE file for copyright and license details. */
#include "exception.h"

#include "proc.h"
#include "trap.h"

void handle_exception(uint64_t mcause, uint64_t mepc, uint64_t mtval)
{
	current->regs.cause = mcause;
	current->regs.epc = mepc;
	current->regs.tval = mtval;
	current->regs.pc = current->regs.tvec;
}
