#include "kernel.h"

#include "altc/altio.h"
#include "altc/init.h"
#include "cap_table.h"
#include "csr.h"
#include "kassert.h"
#include "mcslock.h"
#include "proc.h"
#include "sched.h"

static mcslock_t lock;
static qnode_t nodes[S3K_MAX_HART + 1];
static uint64_t wcet;

void kernel_init(void)
{
	alt_init();
	kprintf(0, "> uart initialized\n");
	mcslock_init(&lock);
	kprintf(0, "> mcslock initialized\n");
	ctable_init();
	kprintf(0, "> ctable initialized\n");
	sched_init();
	kprintf(0, "> scheduler initialized\n");
	proc_init();
	kprintf(0, "> processes initialized\n");
	kprintf(0, "> kernel initialization complete\n");
	kprintf(0, "> starting boot process\n");
}

uint64_t kernel_wcet(void)
{
	return wcet;
}

void kernel_wcet_reset(void)
{
	wcet = 0;
}

bool kernel_lock_acquire(void)
{
	uint64_t i = csrr(mhartid);
	return mcslock_try_acquire(&lock, &nodes[i]);
}

void kernel_lock_release(void)
{
	uint64_t i = csrr(mhartid);
	mcslock_release(&lock, &nodes[i]);
}

bool kernel_preempt(void)
{
#ifndef NPREEMPT
	return csrr(mip) & 0x80;
#else
	return false;
#endif
}

void kernel_preempt_enable(void)
{
#ifndef NPREEMPT
	csrs(mstatus, MSTATUS_MIE);
#endif
}

void kernel_preempt_disable(void)
{
#ifndef NPREEMPT
	csrc(mstatus, MSTATUS_MIE);
#endif
}
