#include "kernel.h"

#include "altc/altio.h"
#include "altc/init.h"
#include "cap_table.h"
#include "csr.h"
#include "current.h"
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
	mcslock_init(&lock);
	ctable_init();
	sched_init();
	proc_init();
	alt_puts("kernel initialized");
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
	uint64_t i = csrr_mhartid();
	return mcslock_try_acquire(&lock, &nodes[i]);
}

void kernel_lock_release(void)
{
	uint64_t i = csrr_mhartid();
	mcslock_release(&lock, &nodes[i]);
}

bool kernel_preempt(void)
{
	return csrr_mip();
}

void kernel_preempt_enable(void)
{
	csrs_mstatus(MSTATUS_MIE);
}

void kernel_preempt_disable(void)
{
	csrc_mstatus(MSTATUS_MIE);
}
