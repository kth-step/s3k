#include "exception.h"

#include "current.h"

enum exception_cause {
	INSTRUCTION_ADDRESS_MISALIGNED = 0,
	INSTRUCTION_ACCESS_FAULT = 1,
	ILLEGAL_INSTRUCTION = 2,
	BREAKPOINT = 3,
	LOAD_ADDRESS_MISALIGNED = 4,
	LOAD_ACCESS_FAULT = 5,
	STORE_AMO_ADDRESS_MISALIGNED = 6,
	STORE_AMO_ACCESS_FAULT = 7,
	ECALL_U = 8,
	ECALL_S = 9,
	ECALL_M = 11,
	INSTRUCTION_PAGE_FAULT = 12,
	LOAD_PAGE_FAULT = 13,
	STORE_AMO_PAGE_FAULT = 15,
};

/* mret instruction encoding */
#define MRET 0x30200073

/**
 * Handle an mret instruction.
 */
proc_t *_handle_mret(void)
{
	current->regs.pc = current->trap.mepc;
	current->trap.mcause = 0;
	current->trap.mtval = 0;
	current->trap.mepc = 0;
	return current;
}

/**
 * Delegates an exception to the current process.
 */
proc_t *_handle_delegate(word_t cause, word_t tval)
{
	current->trap.mcause = cause;
	current->trap.mtval = tval;
	current->trap.mepc = current->regs.pc;
	current->regs.pc = current->trap.mtvec;
	return current;
}

proc_t *exception_handler(word_t cause, word_t tval)
{
	// If mret instruction
	if (cause == ILLEGAL_INSTRUCTION && tval == MRET) {
		return _handle_mret();
	}
	// If not mret instruction
	return _handle_delegate(cause, tval);
}
