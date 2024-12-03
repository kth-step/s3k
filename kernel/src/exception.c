/* See LICENSE file for copyright and license details. */
#include "exception.h"

#include "proc.h"

#define ILLEGAL_INSTRUCTION 0x2

#define MRET 0x30200073
#define SRET 0x10200073
#define URET 0x00200073

static proc_t *_exception_delegate(proc_t *proc, val_t mcause,
				   val_t mtval)
{
	proc->regs[REG_ECAUSE] = mcause;
	proc->regs[REG_EVAL] = mtval;
	proc->regs[REG_EPC] = proc->regs[REG_PC];
	proc->regs[REG_ESP] = proc->regs[REG_SP];
	proc->regs[REG_PC] = proc->regs[REG_TPC];
	proc->regs[REG_SP] = proc->regs[REG_TSP];
	return proc;
}

static proc_t *_exception_trap_return(proc_t *proc)
{
	proc->regs[REG_PC] = proc->regs[REG_EPC];
	proc->regs[REG_SP] = proc->regs[REG_ESP];
	proc->regs[REG_ECAUSE] = 0;
	proc->regs[REG_EVAL] = 0;
	proc->regs[REG_EPC] = 0;
	proc->regs[REG_ESP] = 0;
	return proc;
}

proc_t *exception_handler(proc_t *proc, val_t mcause, val_t mtval)
{
	if (mcause == ILLEGAL_INSTRUCTION
	    && (mtval == MRET || mtval == SRET || mtval == URET))
		return _exception_trap_return(proc);
	return _exception_delegate(proc, mcause, mtval);
}
