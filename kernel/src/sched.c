/* See LICENSE file for copyright and license details. */

#include "sched.h"

#include "csr.h"
#include "drivers/time.h"
#include "kassert.h"
#include "kernel.h"
#include "kprintf.h"
#include "proc.h"
#include "semaphore.h"
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

void sched_update(uint64_t pid, uint64_t end, uint64_t hart, uint64_t from,
		  uint64_t to)
{
	// Acquire all resources, blocking everyone else.
	semaphore_acquire_n(&sched_semaphore, S3K_HART_CNT);
	kprintf(1, "> sched_update(pid=%D,end=%D,hart=%D,from=%D,to=%D)\n", pid,
		end, hart, from, to);
	hart -= S3K_MIN_HART;
	int offset = hart * 16;
	uint64_t mask = 0xFFFFull << offset;
	for (uint64_t i = from; i < to; i++) {
		slots[i] &= ~mask;
		slots[i] |= ((pid << 8) | (end - from)) << offset;
	}
	// Release the resources.
	semaphore_release_n(&sched_semaphore, S3K_HART_CNT);
}

void sched_delete(uint64_t hart, uint64_t from, uint64_t to)
{
	semaphore_acquire_n(&sched_semaphore, S3K_HART_CNT);
	kprintf(1, "> sched_delete(hart=%D,from=%D,to=%D)\n", hart, from, to);
	hart -= S3K_MIN_HART;
	int offset = hart * 16;
	uint64_t mask = 0xFFFFull << offset;
	for (uint64_t i = from; i < to; ++i)
		slots[i] &= ~mask;
	// Release the resources.
	semaphore_release_n(&sched_semaphore, S3K_HART_CNT);
}

slot_info_t slot_info_get(uint64_t hartid, uint64_t slot)
{
	uint64_t x = slots[slot % S3K_SLOT_CNT];
	x >>= (hartid - S3K_MIN_HART) * 16;
	x &= 0xFFFF;
	return (slot_info_t){.pid = (x >> 8) & 0xFF, .length = x & 0xFF};
}

static proc_t *sched_fetch(uint64_t hart, uint64_t slot)
{
	// Get time slot information
	slot_info_t si = slot_info_get(hart, slot);

	// If length = 0, then slice is deleted.
	if (si.length == 0)
		return NULL;

	// Have priority over harts with lower ID when scheduling length is
	// longer.
	for (uint64_t i = S3K_MIN_HART; i < hart; i++) {
		slot_info_t other_si = slot_info_get(i, slot);
		if (si.pid == other_si.pid && si.length <= other_si.length)
			return NULL;
	}

	// Have priority over harts with higher ID when scheduling length is
	// equal or longer.
	for (uint64_t i = hart + 1; i < S3K_MAX_HART; i++) {
		slot_info_t other_si = slot_info_get(i, slot);
		if (si.pid == other_si.pid && si.length < other_si.length)
			return NULL;
	}

	// Get the process.
	proc_t *p = proc_get(si.pid);

	// Try to acquire the process.
	if (!proc_acquire(p))
		return NULL;
	kprintf(2, "> sched(hart=%d,pid=%d,slot=%D)\n", hart, si.pid,
		slot % S3K_SLOT_CNT);
	p->timeout = (slot + si.length) * S3K_SLOT_LEN;
	return p;
}

proc_t *sched(void)
{
	// Hart ID
	uint64_t hart = csrr(mhartid);
	// Time slot
	uint64_t slot = (time_get() + S3K_SCHED_TIME) / S3K_SLOT_LEN;
	// Process to run
	proc_t *next;

	do {
		while (time_get() < slot * S3K_SLOT_LEN)
			;
		// Try schedule process
		semaphore_acquire(&sched_semaphore);
		next = sched_fetch(hart, slot);
		semaphore_release(&sched_semaphore);
		slot++;
	} while (!next);

	timeout_set(hart, next->timeout - S3K_SCHED_TIME);

	return next;
}
