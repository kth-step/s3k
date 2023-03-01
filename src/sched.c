/* See LICENSE file for copyright and license details. */

#include "sched.h"

#include "bsp/timer.h"
#include "consts.h"
#include "csr.h"
#include "proc.h"
#include "trap.h"

#define NONE_PID 0xFF

struct sched_entry {
	uint8_t pid;
	uint8_t len;
};

static volatile struct sched_entry schedule[4][NSLICE];

static struct sched_entry sched_get(uint64_t hartid, size_t i)
{
	return schedule[hartid][i];
}

void sched_update(uint64_t hartid, uint64_t pid, uint64_t begin, uint64_t end)
{
	for (uint64_t i = begin; i < end; i++) {
		schedule[hartid][i] = (struct sched_entry){ pid, end - i };
	}
}

void sched_delete(uint64_t hartid, uint64_t begin, uint64_t end)
{
	sched_update(hartid, NONE_PID, begin, end);
}

void sched_next(void)
{
	uint64_t hartid = csrr_mhartid();
	uint64_t quantum;
	struct sched_entry entry;
retry:
	do {
		quantum = (time_get() + NSLACK) / NTICK;
		entry = sched_get(hartid, quantum % NSLICE);
		if (entry.pid == NONE_PID)
			continue;
		current = &processes[entry.pid];
		if (current->sleep > time_get())
			continue;
	} while (!__sync_bool_compare_and_swap(&current->state, PS_READY, PS_RUNNING));
	proc_load_pmp(current);
	if (!csrr_pmpcfg0()) { // Temporary fix. QEMU does not allow this to be zero.
		__atomic_fetch_and(&current->state, ~PS_RUNNING, __ATOMIC_RELEASE);
		goto retry;
	}
	timeout_set(hartid, quantum * NTICK);
	while (!(csrr_mip() & (1 << 7))) {
		__asm__ volatile("wfi");
	}
	timeout_set(hartid, (quantum + entry.len) * NTICK - NSLACK);
}

void sched_yield(void)
{
	__atomic_fetch_and(&current->state, ~PS_RUNNING, __ATOMIC_RELEASE);
	current = NULL;
	sched_next();
}

void sched_init(void)
{
	for (int i = 0; i < NHART; i++) {
		sched_update(i, 0, 0, NSLICE);
	}
}
