/* See LICENSE file for copyright and license details. */

#include "schedule.h"

#include "consts.h"
#include "csr.h"
#include "current.h"
#include "proc.h"
#include "timer.h"
#include "trap.h"
#include "wfi.h"

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

void schedule_update(uint64_t hartid, uint64_t pid, uint64_t begin,
		     uint64_t end)
{
	for (uint64_t i = begin; i < end; i++) {
		schedule[hartid][i] = (struct sched_entry){ pid, end - i };
	}
}

void schedule_delete(uint64_t hartid, uint64_t begin, uint64_t end)
{
	schedule_update(hartid, NONE_PID, begin, end);
}

void schedule_next()
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
	} while (proc_acquire(proc, PS_READY));
	proc_load_pmp(proc);
	if (!csrr_pmpcfg0()) {
		// Temporary fix. QEMU does not allow this to be zero.
		proc_release(proc);
		goto retry;
	}
	timeout_set(hartid, quantum * NTICK);
	while (!(csrr_mip() & (1 << 7))) {
		wfi();
	}
	timeout_set(hartid, (quantum + entry.len) * NTICK - NSLACK);
	current_set(proc);
}

void schedule_yield(struct proc *proc)
{
	proc_release(proc);
	schedule_next();
}

void schedule_init(void)
{
	for (int i = 0; i < NHART; i++) {
		schedule_update(i, 0, 0, NSLICE);
	}
}
