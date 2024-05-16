/* See LICENSE file for copyright and license details. */

#include "sched.h"

#include "csr.h"
#include "drivers/time.h"
#include "kassert.h"
#include "kernel.h"
#include "kprintf.h"
#include "proc.h"
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

static uint64_t slots[S3K_SLOT_CNT];

#ifdef SMP
static semaphore_t sched_semaphore;
#endif

void sched_init(void)
{
	uint64_t pid = 0;
	uint64_t end = S3K_SLOT_CNT;
	uint64_t from = 0;
	uint64_t to = S3K_SLOT_CNT;

#ifdef SMP
	semaphore_init(&sched_semaphore, S3K_HART_CNT);
#endif

	for (uint64_t hartid = S3K_MIN_HART; hartid <= S3K_MAX_HART; hartid++)
		sched_update(pid, end, hartid, from, to);
}

void sched_update(uint64_t pid, uint64_t end, uint64_t hart, uint64_t from,
		  uint64_t to)
{
	kprintf(1, "# sched_update(pid=%D,end=%D,hart=%D,from=%D,to=%D)\n", pid,
		end, hart, from, to);
#ifdef SMP
	semaphore_acquire_n(&sched_semaphore, S3K_HART_CNT);
#endif
	hart -= S3K_MIN_HART;
	int offset = hart * 16;
	uint64_t mask = 0xFFFFull << offset;
	for (uint64_t i = from; i < to; i++) {
		slots[i] &= ~mask;
		slots[i] |= ((pid << 8) | (end - i)) << offset;
	}
#ifdef SMP
	semaphore_release_n(&sched_semaphore, S3K_HART_CNT);
#endif
}

void sched_delete(uint64_t hart, uint64_t from, uint64_t to)
{
	kprintf(1, "# sched_delete(hart=%D,from=%D,to=%D)\n", hart, from, to);
#ifdef SMP
	semaphore_acquire_n(&sched_semaphore, S3K_HART_CNT);
#endif
	hart -= S3K_MIN_HART;
	int offset = hart * 16;
	uint64_t mask = 0xFFFFull << offset;
	for (uint64_t i = from; i < to; ++i)
		slots[i] &= ~mask;
#ifdef SMP
	semaphore_release_n(&sched_semaphore, S3K_HART_CNT);
#endif
}

static slot_info_t slot_info_get(uint64_t hart, uint64_t slot)
{
	uint64_t entry = slots[slot % S3K_SLOT_CNT]
			 >> (hart - S3K_MIN_HART) * 16;
	uint64_t pid = (entry >> 8) & 0xFF;
	uint64_t length = entry & 0xFF;
	return (slot_info_t){.pid = pid, .length = length};
}

static proc_t *sched_fetch(uint64_t hart, uint64_t slot)
{
	proc_t *proc = NULL;
#ifdef SMP
	semaphore_acquire(&sched_semaphore);
#endif
	// Get time slot information
	slot_info_t si = slot_info_get(hart, slot);

	// If length = 0, then slice is deleted.
	if (si.length == 0)
		goto fail;

#ifdef SMP
	// Have priority over harts with lower ID when scheduling length is
	// longer.
	for (uint64_t i = S3K_MIN_HART; i < hart; i++) {
		slot_info_t other_si = slot_info_get(i, slot);
		if (si.pid == other_si.pid && si.length <= other_si.length)
			goto fail;
	}

	// Have priority over harts with higher ID when scheduling length is
	// equal or longer.
	for (uint64_t i = hart + 1; i < S3K_MAX_HART; i++) {
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

	// Get the process.
	kprintf(2, "# sched(hart=%d,pid=%d,slot=%D)\n", hart, si.pid,
		slot % S3K_SLOT_CNT);
	proc->timeout = (slot + si.length) * S3K_SLOT_LEN - S3K_SCHED_TIME;
fail:
#ifdef SMP
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
	timeout_set(hart, (uint64_t)-1);

	do {
		slot = (time_get() + S3K_SCHED_TIME) / S3K_SLOT_LEN;
		while (time_get() < slot * S3K_SLOT_LEN)
			;
		// Try schedule process
		proc = sched_fetch(hart, slot);
	} while (!proc);
	timeout_set(hart, proc->timeout);
	return proc;
}
