#include "kernel.h"

#include "altc/init.h"
#include "cap_lock.h"
#include "cap_table.h"
#include "csr.h"
#include "kassert.h"
#include "proc.h"
#include "sched.h"

__attribute__((section(".header"))) const unsigned long header_data[2] = {0xaedb041d, 0xaedb041d};

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

bool kernel_preempt(void)
{
	return csrr(mip) & 0x80;
}
