/* See LICENSE file for copyright and license details. */
#include "exception.h"

#include "proc.h"
#include "trap.h"

#define ILLEGAL_INSTRUCTION 0x2

#define MRET 0x30200073
#define SRET 0x10200073
#define URET 0x00200073

static void handle_ret(struct proc *proc)
{
	proc->regs[REG_PC] = proc->regs[REG_EPC];
	proc->regs[REG_SP] = proc->regs[REG_ESP];
	proc->regs[REG_ECAUSE] = 0;
	proc->regs[REG_EVAL] = 0;
	proc->regs[REG_EPC] = 0;
	proc->regs[REG_ESP] = 0;
}

static void handle_default(struct proc *proc, uint64_t mcause, uint64_t mepc,
			   uint64_t mtval)
{
	proc->regs[REG_ECAUSE] = mcause;
	proc->regs[REG_EVAL] = mtval;
	proc->regs[REG_EPC] = mepc;
	proc->regs[REG_ESP] = proc->regs[REG_SP];
	proc->regs[REG_PC] = proc->regs[REG_TPC];
	proc->regs[REG_SP] = proc->regs[REG_TSP];
}

void handle_exception(struct proc *proc, uint64_t mcause, uint64_t mepc,
		      uint64_t mtval)
{
	if (mcause == ILLEGAL_INSTRUCTION
	    && (mtval == MRET || mtval == SRET || mtval == URET))
		handle_ret(proc);
	else
		handle_default(proc, mcause, mepc, mtval);
}
