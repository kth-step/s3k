#include "kernel.h"

#include "altc/altio.h"
#include "altc/init.h"
#include "cap_lock.h"
#include "cap_table.h"
#include "csr.h"
#include "kassert.h"
#include "proc.h"
#include "sched.h"

#ifdef INSTRUMENT_WCRT
uint64_t kernel_wcrt;
#endif

void kernel_init(void)
{
	alt_init();
	kprintf(0, "# uart initialized\n");
#ifdef SMP
	cap_lock_init();
	kprintf(0, "# capability lock initialized\n");
#endif
	ctable_init();
	kprintf(0, "# ctable initialized\n");
	sched_init();
	kprintf(0, "# scheduler initialized\n");
	proc_init();
	kprintf(0, "# processes initialized\n");
	kprintf(0, "# kernel initialization complete\n");
	kprintf(0, "# starting boot process\n");
}

uint64_t kernel_wcet(void)
{
#ifdef INSTRUMENT_WCRT
	return kernel_wcrt;
#else
	return 0;
#endif
}

uint64_t kernel_wcet_reset(void)
{
#ifdef INSTRUMENT_WCRT
	return __atomic_exchange_n(&kernel_wcrt, 0, __ATOMIC_RELAXED);
#else
	return 0;
#endif
}

bool kernel_preempt(void)
{
#ifndef NPREEMPT
#if defined(INSTRUMENT_WCRT)
	if (csrr(mip) & 0x80)
		return true;
	csrw(mcycle, 0);
	return false;
#else
	return csrr(mip) & 0x80;
#endif
#else  /* NPREEMPT */
	return false;
#endif /* NPREEMPT */
}
