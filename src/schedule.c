/* See LICENSE file for copyright and license details. */

#include "schedule.h"

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

static struct sched_entry schedule_get(uint64_t hartid, size_t i)
{
	return schedule[hartid][i];
}

void schedule_update(uint64_t hartid, uint64_t pid, uint64_t begin, uint64_t end)
{
	for (uint64_t i = begin; i < end; i++) {
		schedule[hartid][i] = (struct sched_entry){ pid, end - i };
	}
}

void schedule_delete(uint64_t hartid, uint64_t begin, uint64_t end)
{
	schedule_update(hartid, NONE_PID, begin, end);
}

struct proc *schedule_next()
{
	uint64_t hartid = csrr_mhartid();
	uint64_t quantum;
	struct proc *proc;
	struct sched_entry entry;
retry:
	do {
		quantum = (time_get() + NSLACK) / NTICK;
		entry = schedule_get(hartid, quantum % NSLICE);
		if (entry.pid == NONE_PID)
			continue;
		proc = &processes[entry.pid];
		if (proc->sleep > time_get())
			continue;
	} while (!__sync_bool_compare_and_swap(&proc->state, PS_READY, PS_RUNNING));
	proc_load_pmp(proc);
	if (!csrr_pmpcfg0()) { // Temporary fix. QEMU does not allow this to be zero.
		__atomic_fetch_and(&proc->state, ~PS_RUNNING, __ATOMIC_RELEASE);
		goto retry;
	}
	timeout_set(hartid, quantum * NTICK);
	while (!(csrr_mip() & (1 << 7))) {
		__asm__ volatile("wfi");
	}
	timeout_set(hartid, (quantum + entry.len) * NTICK - NSLACK);
	return proc;
}

struct proc *schedule_yield(struct proc *proc)
{
	__atomic_fetch_and(&proc->state, ~PS_RUNNING, __ATOMIC_RELEASE);
	return schedule_next();
}

void schedule_init(void)
{
	for (int i = 0; i < NHART; i++) {
		schedule_update(i, 0, 0, NSLICE);
	}
}
