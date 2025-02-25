#include "kernel.h"

#include "altc/init.h"
#include "cap/lock.h"
#include "cap/table.h"
#include "csr.h"
#include "kassert.h"
#include "proc.h"
#include "sched.h"

void kernel_init(void)
{
	kprintf(0, "# uart initialized\n");
#if NHART > 1
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

bool kernel_preempt(void)
{
	return csrr(mip) & 0x80;
}
