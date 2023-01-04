/* See LICENSE file for copyright and license details. */
#include "proc.h"

struct proc *handle_exception(struct proc *proc, uint64_t mcause, uint64_t mepc, uint64_t mtval)
{
	proc->regs.cause = mcause;
	proc->regs.epc = mepc;
	proc->regs.tval = mtval;
	proc->regs.pc = proc->regs.tvec;
	return proc;
}
