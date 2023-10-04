/* See LICENSE file for copyright and license details. */

#include "sched.h"

#include "csr.h"
#include "drivers/timer.h"
#include "kassert.h"
#include "kernel.h"
#include "proc.h"
#include "semaphore.h"
#include "trap.h"
#include "wfi.h"

typedef struct slot_info {
	// Owner of time slot.
	uint32_t pid;
	// Remaining length of corresponding slice.
	uint32_t length;
} slot_info_t;

static slot_info_t slots[S3K_HART_CNT][S3K_SLOT_CNT];
static semaphore_t sched_semaphore;

void sched_init(void)
{
	uint64_t pid = 0;
	uint64_t end = S3K_SLOT_CNT;
	uint64_t from = 0;
	uint64_t to = S3K_SLOT_CNT;

	semaphore_init(&sched_semaphore, S3K_HART_CNT);

	for (uint64_t hartid = S3K_MIN_HART; hartid <= S3K_MAX_HART; hartid++)
		sched_update(pid, end, hartid, from, to);
}

void sched_update(uint64_t pid, uint64_t end, uint64_t hartid, uint64_t from,
		  uint64_t to)
{
	// Acquire all resources, blocking everyone else.
	semaphore_acquire_n(&sched_semaphore, S3K_HART_CNT);
	for (uint64_t i = from; i < to; i++) {
		slots[hartid - S3K_MIN_HART][i].pid = pid & 0xFF;
		slots[hartid - S3K_MIN_HART][i].length = (end - i) & 0xFF;
	}
	// Release the resources.
	semaphore_release_n(&sched_semaphore, S3K_HART_CNT);
}

void sched_delete(uint64_t hartid, uint64_t from, uint64_t to)
{
	semaphore_acquire_n(&sched_semaphore, S3K_HART_CNT);
	for (uint64_t i = from; i < to; ++i) {
		slots[hartid - S3K_MIN_HART][i].pid = 0;
		slots[hartid - S3K_MIN_HART][i].length = 0;
	}
	// Release the resources.
	semaphore_release_n(&sched_semaphore, S3K_HART_CNT);
}

slot_info_t slot_info_get(uint64_t hartid, uint64_t slot)
{
	return slots[hartid - S3K_MIN_HART][slot % S3K_SLOT_CNT];
}

static proc_t *sched_fetch(uint64_t hartid, uint64_t *start_time,
			   uint64_t *end_time)
{
	semaphore_acquire(&sched_semaphore);

	// Get time slot (in global sense)
	uint64_t slot = time_get() / S3K_SLOT_LEN;
	// Get time slot information
	slot_info_t si = slot_info_get(hartid, slot);
	// Check if time slot is first in time slice.
	bool first = (slot_info_get(hartid, slot - 1).length == 0);

	// If length = 0, then slice is deleted.
	if (si.length == 0)
		goto fail;

	// Have priority over harts with lower ID when scheduling length is
	// longer.
	for (uint64_t i = S3K_MIN_HART; i < hartid; i++) {
		slot_info_t other_si = slot_info_get(i, slot);
		if (si.pid == other_si.pid && si.length <= other_si.length)
			goto fail;
	}

	// Have priority over harts with higher ID when scheduling length is
	// equal or longer.
	for (uint64_t i = hartid + 1; i < S3K_MAX_HART; i++) {
		slot_info_t other_si = slot_info_get(i, slot);
		if (si.pid == other_si.pid && si.length < other_si.length)
			goto fail;
	}

	// Get the process.
	proc_t *p = proc_get(si.pid);

	// Try to acquire the process.
	if (!proc_acquire(p))
		goto fail;
	semaphore_release(&sched_semaphore);
	*start_time = slot * S3K_SLOT_LEN + (first ? S3K_SCHED_TIME : 0);
	*end_time = (slot + si.length) * S3K_SLOT_LEN;
	p->timeout = *end_time;
	return p;

fail:
	semaphore_release(&sched_semaphore);
	return NULL;
}

void sched(proc_t *p)
{
	uint64_t hartid = csrr_mhartid();
	uint64_t start_time, end_time;
	if (p)
		proc_release(p);

	do {
		p = sched_fetch(hartid, &start_time, &end_time);
	} while (!p);

	kernel_pmp_refresh();
	timer_set(hartid, start_time);
	while (!(csrr_mip() & MIP_MTIP))
		wfi();
	timer_set(hartid, end_time);
	trap_exit(p);
}
