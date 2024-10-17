/* See LICENSE file for copyright and license details. */

#include "kern/sched.h"

#include "drivers/time.h"
#include "kern/csr.h"
#include "kern/kassert.h"
#include "kern/kprintf.h"
#include "kern/proc.h"
#include "smp/semaphore.h"
#include "kern/trap.h"
#include "kern/wfi.h"

typedef struct slot_info {
	// Owner of time slot.
	uint32_t pid;
	// Remaining length of corresponding slice.
	uint32_t len;
} slot_info_t;

static slot_info_t slots[S3K_SLOT_CNT];

void sched_init(void)
{
	sched_update(0, S3K_SLOT_CNT, 0, 0, S3K_SLOT_CNT);
}

void sched_update(uint64_t pid, uint64_t end, uint64_t hart, uint64_t from,
		  uint64_t to)
{
	for (uint64_t i = from; i < to; i++) {
		slots[i].pid = pid;
		slots[i].len = end - i;
	}
}

void sched_delete(uint64_t hart, uint64_t from, uint64_t to)
{
	for (uint64_t i = from; i < to; ++i)
		slots[i].len = 0;
}

static slot_info_t slot_info_get(uint64_t hart, uint64_t slot)
{
	return slots[slot % S3K_SLOT_CNT];
}

static proc_t *sched_fetch(uint64_t hart, uint64_t slot)
{
	// Get time slot information
	slot_info_t si = slot_info_get(hart, slot);
	proc_t *proc = proc_get(si.pid);

	// If length = 0, then slice is deleted.
	if (si.len == 0)
		return NULL;

	// Try to acquire the process.
	if (!proc_acquire(proc))
		return NULL;

	// Get the process.
	proc->timeout = (slot + si.len) * S3K_SLOT_LEN;
	return proc;
}

void sched(void)
{
	// Hart ID
	uint64_t hart = csrr(mhartid);
	// Process to schedule
	proc_t *proc;

	timeout_set(hart, (uint64_t)-1);

	do {
		uintptr_t slot = time_get() / S3K_SLOT_LEN;
		while (time_get() < slot * S3K_SLOT_LEN)
			;
		// Try schedule process
		proc = sched_fetch(hart, slot);
	} while (!proc);
	timeout_set(hart, proc->timeout);
	trap_resume(proc);
}
