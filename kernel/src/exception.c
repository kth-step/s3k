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

proc_t *handle_exception(proc_t *p)
{
	/* Check if it is a return from exception */
	p->regs[REG_ECAUSE] = csrr_mcause();
	p->regs[REG_EVAL] = csrr_mtval();
	p->regs[REG_EPC] = p->regs[REG_PC];
	p->regs[REG_ESP] = p->regs[REG_SP];
	p->regs[REG_PC] = p->regs[REG_TPC];
	p->regs[REG_SP] = p->regs[REG_TSP];
	if (!p->regs[REG_PC]) {
		proc_suspend(p);
		return NULL;
	}
	return p;
}

static proc_t *handle_trap_return(proc_t *p)
{
	p->regs[REG_PC] = p->regs[REG_EPC];
	p->regs[REG_SP] = p->regs[REG_ESP];
	p->regs[REG_ECAUSE] = 0;
	p->regs[REG_EVAL] = 0;
	p->regs[REG_EPC] = 0;
	p->regs[REG_ESP] = 0;
	return p;
}

proc_t *handle_illegal_instruction(proc_t *p)
{
	switch (csrr_mtval()) {
	case MRET:
	case SRET:
	case URET:
		return handle_trap_return(p);
	default:
		return handle_exception(p);
	}
}
