/* See LICENSE file for copyright and license details. */
#include "sched.h"

#include "consts.h"
#include "csr.h"
#include "proc.h"
#include "timer.h"

struct sched_entry {
	uint8_t pid;
	uint64_t len;
};

static uint16_t schedule[4][NSLICE];

struct sched_entry schedule_get(uint64_t hartid, size_t i)
{
	uint16_t entry = schedule[hartid][i];
	return (struct sched_entry){(uint8_t)entry & 0xFFull, (entry >> 8) + 1};
}

void schedule_set(uint64_t hartid, size_t i, struct sched_entry entry)
{
	schedule[hartid][i] = (uint16_t)entry.pid | ((uint16_t)(entry.len - 1) << 8);
}

void schedule_update(uint64_t hartid, uint64_t pid, uint64_t begin, uint64_t end)
{
	static struct ticklock lock = {0};
	ticklock_lock(&lock);
	while (begin < end) {
		struct sched_entry entry = (struct sched_entry){(uint8_t)pid, end - begin};
		schedule_set(hartid, begin, entry);
		begin++;
	}
	ticklock_unlock(&lock);
}

struct proc *schedule_next(void)
{
	uint64_t hartid = csrr_mhartid();
	uint64_t state;
	uint64_t quantum;
	struct sched_entry entry;
	struct proc *proc;
	do {
		quantum = (timer_gettime() - NSLACK) / NTICK;
		entry = schedule_get(quantum % NTICK, hartid);
		if (entry.pid == 0xFFull)
			continue;
		proc = &processes[entry.pid];
		state = proc->state;
		if (state != PS_READY)
			continue;
	} while (!__atomic_compare_exchange_n(&proc->state, &state, PS_RUNNING, false,
					      __ATOMIC_ACQUIRE, __ATOMIC_RELAXED));
	proc_loadpmp(proc);
	uint64_t endtime = (quantum + entry.len) * NTICK;
	timer_settimer(hartid, endtime);
	return proc;
}

struct proc *schedule_yield(struct proc *proc)
{
	__atomic_fetch_and(&proc->state, ~PS_RUNNING, __ATOMIC_RELEASE);
	return schedule_next();
}
