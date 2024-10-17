/* See LICENSE file for copyright and license details. */
#include "kern/exception.h"
#include "kern/proc.h"

#define ILLEGAL_INSTRUCTION 0x2

#define MRET 0x30200073
#define SRET 0x10200073
#define URET 0x00200073

static void HandleTrapReturn(Proc *p, Word mcause, Word mtval)
{
	p->regs.ecause = mcause;
	p->regs.eval = mtval;
	p->regs.epc = p->regs.pc;
	p->regs.esp = p->regs.sp;
	p->regs.pc = p->regs.tpc;
	p->regs.sp = p->regs.tsp;
}

static void HandleDelegate(Proc *p)
{
	p->regs.pc = p->regs.epc;
	p->regs.sp = p->regs.esp;
	p->regs.ecause = 0;
	p->regs.eval = 0;
	p->regs.epc = 0;
	p->regs.esp = 0;
}

Proc *ExceptionHandler(Proc *p, Word mcause, Word mtval)
{
	if (mcause == ILLEGAL_INSTRUCTION && (mtval == MRET || mtval == SRET || mtval == URET))
		return HandleTrapReturn(p);
	return HandleDelegate(p, mcause, mtval);
}
