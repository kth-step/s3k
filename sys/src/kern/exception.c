/* See LICENSE file for copyright and license details. */
#include "kern/exception.h"

#include "kern/proc.h"

#define ILLEGAL_INSTRUCTION 0x2

#define MRET 0x30200073
#define SRET 0x10200073
#define URET 0x00200073

static void _exception_delegate(uint64_t mcause, uint64_t mtval)
{
	current->regs.ecause = mcause;
	current->regs.eval = mtval;
	current->regs.epc = current->regs.pc;
	current->regs.esp = current->regs.sp;
	current->regs.pc = current->regs.tpc;
	current->regs.sp = current->regs.tsp;
}

static void _exception_trap_return(void)
{
	current->regs.pc = current->regs.epc;
	current->regs.sp = current->regs.esp;
	current->regs.ecause = 0;
	current->regs.eval = 0;
	current->regs.epc = 0;
	current->regs.esp = 0;
}

void exception_handler(uint64_t mcause, uint64_t mtval)
{
	if (mcause == ILLEGAL_INSTRUCTION
	    && (mtval == MRET || mtval == SRET || mtval == URET))
		_exception_trap_return();
	else
		_exception_delegate(mcause, mtval);
}
