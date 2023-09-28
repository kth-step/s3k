#include "kernel.h"

#include "cap_table.h"
#include "csr.h"
#include "drivers/uart.h"
#include "kassert.h"
#include "mcslock.h"
#include "proc.h"
#include "sched.h"

static mcslock_t lock;
static qnode_t nodes[S3K_HART_CNT];

void kernel_init(void)
{
	mcslock_init(&lock, nodes);
	uart_init();
	ctable_init();
	sched_init();
	proc_init();
	uart_puts("Kernel initialization complete");
}

bool kernel_lock(void)
{
	return mcslock_try_acquire(&lock, csrr_mhartid() - S3K_MIN_HART);
}

void kernel_unlock(void)
{
	mcslock_release(&lock, csrr_mhartid() - S3K_MIN_HART);
}
