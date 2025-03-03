/* See LICENSE file for copyright and license details. */

#include "sched.h"

#include "csr.h"
#include "kassert.h"
#include "kernel.h"
#include "kprintf.h"
#include "proc.h"
#include "rtc.h"
#include "smp/semaphore.h"
#include "trap.h"
#include "wfi.h"

typedef struct slot_info {
	// Owner of time slot.
	uint8_t pid;
	// Remaining length of corresponding slice.
	uint8_t length;
} slot_info_t;

struct sched_decision {
	proc_t *proc;
	uint64_t end_time;
};

static uint64_t slots[NSLOT];

#if NHART > 1
static semaphore_t sched_semaphore;
#endif

void sched_init(void)
{
	uint64_t pid = 0;
	uint64_t end = NSLOT;
	uint64_t from = 0;
	uint64_t to = NSLOT;

#if NHART > 1
	semaphore_init(&sched_semaphore, NHART);
#endif

	for (uint64_t hartid = 0; hartid < NHART; hartid++)
		sched_update(pid, end, hartid, from, to);
}

void sched_update(uint64_t pid, uint64_t end, uint64_t hart, uint64_t from,
		  uint64_t to)
{
#if NHART > 1
	semaphore_acquire_n(&sched_semaphore, NHART);
#endif
	int offset = hart * 16;
	uint64_t mask = 0xFFFFull << offset;
	for (uint64_t i = from; i < to; i++) {
		slots[i] &= ~mask;
		slots[i] |= ((pid << 8) | (end - i)) << offset;
	}
#if NHART > 1
	semaphore_release_n(&sched_semaphore, NHART);
#endif
}

void sched_delete(uint64_t hart, uint64_t from, uint64_t to)
{
#if NHART > 1
	semaphore_acquire_n(&sched_semaphore, NHART);
#endif
	int offset = hart * 16;
	uint64_t mask = 0xFFFFull << offset;
	for (uint64_t i = from; i < to; ++i)
		slots[i] &= ~mask;
#if NHART > 1
	semaphore_release_n(&sched_semaphore, NHART);
#endif
}

static slot_info_t slot_info_get(uint64_t hart, uint64_t slot)
{
	uint64_t entry = slots[slot % NSLOT] >> hart * 16;
	uint64_t pid = (entry >> 8) & 0xFF;
	uint64_t length = entry & 0xFF;
	return (slot_info_t){.pid = pid, .length = length};
}

static proc_t *sched_fetch(uint64_t hart, uint64_t slot)
{
	proc_t *proc = NULL;
#if NHART > 1
	semaphore_acquire(&sched_semaphore);
#endif
	// Get time slot information
	slot_info_t si = slot_info_get(hart, slot);

	// If length = 0, then slice is deleted.
	if (si.length == 0)
		goto fail;

#if NHART > 1
	// Have priority over harts with lower ID when scheduling length is
	// longer.
	for (uint64_t i = 0; i < hart; i++) {
		slot_info_t other_si = slot_info_get(i, slot);
		if (si.pid == other_si.pid && si.length <= other_si.length)
			goto fail;
	}

	// Have priority over harts with higher ID when scheduling length is
	// equal or longer.
	for (uint64_t i = hart + 1; i < NHART; i++) {
		slot_info_t other_si = slot_info_get(i, slot);
		if (si.pid == other_si.pid && si.length < other_si.length)
			goto fail;
	}
#endif

	proc = proc_get(si.pid);

	// Try to acquire the process.
	if (!proc_acquire(proc)) {
		proc = NULL;
		goto fail;
	}

	proc->timeout = (slot + si.length) * NTICK;
fail:
#if NHART > 1
	semaphore_release(&sched_semaphore);
#endif
	return proc;
}

proc_t *sched(void)
{
	// Hart ID
	uint64_t hart = csrr(mhartid);
	// Time slot
	uint64_t slot;
	// Process to schedule
	proc_t *proc;
	rtc_timeout_set(hart, (uint64_t)-1);

	do {
		slot = rtc_time_get() / NTICK;
		while (rtc_time_get() < slot * NTICK)
			;
		// Try schedule process
		proc = sched_fetch(hart, slot);
	} while (!proc);
	rtc_timeout_set(hart, proc->timeout);
	return proc;
}
