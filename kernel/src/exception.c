/* See LICENSE file for copyright and license details. */
#include "exception.h"

#include "kernel.h"
#include "proc.h"

#define ILLEGAL_INSTRUCTION 0x2

#define MRET 0x30200073
#define SRET 0x10200073
#define URET 0x00200073

static proc_t *handle_ret(proc_t *p);
static proc_t *handle_default(proc_t *p, reg_t mcause, reg_t mepc, reg_t mtval);

proc_t *handle_exception(proc_t *p, reg_t mcause, reg_t mepc, reg_t mtval)
{
	/* Check if it is a return from exception */
	if (mcause == ILLEGAL_INSTRUCTION
	    && (mtval == MRET || mtval == SRET || mtval == URET))
		// Handle return from exception
		return handle_ret(p);
	// Handle default exception
	return handle_default(p, mcause, mepc, mtval);
}

/**
 * This function restores the program counter and stack pointer to their values
 * prior to the exception, and clears the exception cause and exception value
 * registers.
 */
proc_t *handle_ret(proc_t *p)
{
	p->tf.pc = p->tf.epc;
	p->tf.sp = p->tf.esp;
	p->tf.ecause = 0;
	p->tf.eval = 0;
	p->tf.epc = 0;
	p->tf.esp = 0;
	return p;
}

/*
 * This function is called when an exception occurs that doesn't fall under the
 * category of an illegal instruction return, such as a page fault or a timer
 * interrupt. It updates the exception cause, value, program counter, and stack
 * pointer in the process's registers, and switches to the trap handler program
 * counter and stack pointer.
 */
proc_t *handle_default(proc_t *p, uint64_t mcause, uint64_t mepc,
		       uint64_t mtval)
{
	p->tf.ecause = mcause;
	p->tf.eval = mtval;
	p->tf.epc = p->tf.pc;
	p->tf.esp = p->tf.sp;
	p->tf.pc = p->tf.tpc;
	p->tf.sp = p->tf.tsp;
	return p;
}
