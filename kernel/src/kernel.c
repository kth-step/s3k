#include "kernel.h"

#include "cap_table.h"
#include "csr.h"
#include "drivers/uart.h"
#include "kassert.h"
#include "mcslock.h"
#include "proc.h"
#include "sched.h"

static mcslock_t lock;

void kernel_init(void)
{
	mcslock_init(&lock);
	uart_init();
	ctable_init();
	sched_init();
	proc_init();
	uart_puts("Kernel initialization complete");
}

bool kernel_lock(proc_t *p)
{
	kernel_hook_sys_preempt(p);
	return mcslock_try_acquire(&lock, &p->qnode);
}

void kernel_unlock(proc_t *p)
{
	mcslock_release(&lock, &p->qnode);
}

void kernel_pmp_refresh(void)
{
	csrw_pmpcfg0(0);
}

void kernel_hook_sys_entry(proc_t *p)
{
#ifdef INSTRUMENT
	csrw_mcycle(0);
#endif
}

void kernel_hook_sys_exit(proc_t *p)
{
#ifdef INSTRUMENT
	uint64_t cycles = csrr_mcycle();
	if (p->tf.wcet < cycles)
		p->tf.wcet = cycles;
#endif
}

void kernel_hook_sys_preempt(proc_t *p)
{
#ifdef INSTRUMENT
	uint64_t cycles = csrr_mcycle();
	csrw_mcycle(0);
	if (p->tf.wcet < cycles)
		p->tf.wcet = cycles;
#endif
}
