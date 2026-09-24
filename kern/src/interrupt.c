#include "interrupt.h"

#include "current.h"

#ifdef PLATFORM_PREEMPT_STK
/* USBFS CDC interrupt (the only armed PFIC line), serviced in
 * kern/platform/usbfs_cdc.c. */
extern void usbfs_cdc_irq(void);
#endif

proc_t *interrupt_handler(word_t cause, word_t tval)
{
	(void)tval;
#ifdef PLATFORM_PREEMPT_STK
	if (cause == (word_t)(0x80000000u | 11u))
		usbfs_cdc_irq();
	return current;
#else
	(void)cause;
	// Returning NULL invokes the scheduler later.
	return NULL;
#endif
}
