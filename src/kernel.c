#include "kernel.h"

#include "cap/lock.h"
#include "cap/table.h"
#include "csr.h"
#include "kassert.h"
#include "kprintf.h"
#include "proc.h"
#include "sched.h"

void kernel_init(void)
{
	kprintf("# uart initialized\n");
#if NHART > 1
	cap_lock_init();
	kprintf("# capability lock initialized\n");
#endif
	ctable_init();
	kprintf("# ctable initialized\n");
	sched_init();
	kprintf("# scheduler initialized\n");
	proc_init();
	kprintf("# processes initialized\n");
	kprintf("# kernel initialization complete\n");
	kprintf("# starting boot process\n");
}

bool kernel_preempt(void)
{
	return csrr(mip) & 0x80;
}
