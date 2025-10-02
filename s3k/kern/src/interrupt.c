#include "interrupt.h"

/**
 * Dummy interrupt handler.
 */
proc_t *interrupt_handler(word_t cause, word_t tval)
{
	(void)cause;
	(void)tval;
	// Returning NULL invokes the scheduler later.
	return NULL;
}
