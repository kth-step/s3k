/* See LICENSE file for copyright and license details. */

#include "sched.h"

#include "csr.h"
#include "drivers/timer.h"
#include "kassert.h"
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

static slot_info_t slots[N_HART][N_SLOT];
static semaphore_t sched_semaphore;

void sched_init(void)
{
	uint64_t pid = 0;
	uint64_t end = N_SLOT;
	uint64_t from = 0;
	uint64_t to = N_SLOT;

	semaphore_init(&sched_semaphore, N_HART);

	for (uint64_t hartid = MIN_HARTID; hartid <= MAX_HARTID; hartid++)
		sched_update(pid, end, hartid, from, to);
}

void sched_update(uint64_t pid, uint64_t end, uint64_t hartid, uint64_t from, uint64_t to)
{
	// Acquire all resources, blocking everyone else.
	semaphore_acquire_n(&sched_semaphore, N_HART);
	for (uint64_t i = from; i < to; i++) {
		slots[hartid - MIN_HARTID][i].pid = pid & 0xFF;
		slots[hartid - MIN_HARTID][i].length = (end - i) & 0xFF;
	}
	// Release the resources.
	semaphore_release_n(&sched_semaphore, N_HART);
}

void sched_delete(uint64_t hartid, uint64_t from, uint64_t to)
{
	semaphore_acquire_n(&sched_semaphore, N_HART);
	for (uint64_t i = from; i < to; ++i) {
		slots[hartid - MIN_HARTID][i].pid = 0;
		slots[hartid - MIN_HARTID][i].length = 0;
	}
	// Release the resources.
	semaphore_release_n(&sched_semaphore, N_HART);
}

slot_info_t slot_info_get(uint64_t hartid, uint64_t slot)
{
	return slots[hartid - MIN_HARTID][slot % N_SLOT];
}

static proc_t *sched_fetch(uint64_t hartid, uint64_t *start_time, uint64_t *end_time)
{
	proc_t *p = NULL;
	semaphore_acquire(&sched_semaphore);

	uint64_t slot = time_get() / SLOT_LENGTH;
	slot_info_t si = slot_info_get(hartid, slot);

	// If length = 0, then slice is deleted.
	if (si.length == 0)
		goto fail;

	// Have priority over harts with lower ID when scheduling length is
	// longer.
	for (uint64_t i = MIN_HARTID; i < hartid; i++) {
		slot_info_t other_si = slot_info_get(hartid, slot);
		if (si.pid == other_si.pid && si.length <= other_si.length)
			goto fail;
	}

	// Have priority over harts with higher ID when scheduling length is
	// equal or longer.
	for (uint64_t i = hartid + 1; i < MAX_HARTID; i++) {
		slot_info_t other_si = slot_info_get(hartid, slot);
		if (si.pid == other_si.pid && si.length < other_si.length)
			goto fail;
	}

	// Get the process.
	p = proc_get(si.pid);

	// Try to acquire the process.
	if (!proc_acquire(p)) {
		p = NULL;
		goto fail;
	}
	*start_time = slot * SLOT_LENGTH;
	*end_time = (slot + si.length) * SLOT_LENGTH - SCHEDULER_TIME;
	p->timeout = *end_time;
fail:
	semaphore_release(&sched_semaphore);
	return p;
}

proc_t *sched(proc_t *p)
{
	uint64_t hartid = csrr_mhartid();
	uint64_t start_time, end_time;
	if (p)
		proc_release(p);

	do {
		p = sched_fetch(hartid, &start_time, &end_time);
	} while (!p);

	proc_pmp_sync(p);
	timer_set(hartid, start_time);
	while (!(csrr_mip() & MIP_MTIP))
		wfi();
	timer_set(hartid, end_time);
	return p;
}
