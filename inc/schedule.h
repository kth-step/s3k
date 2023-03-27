/**
 * @file sched.h
 * @brief Scheduler.
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 * @bug QEMU mret does not work properly if all pmp registers are 0, so we have
 * a temporary fix in sched_next.
 */
#ifndef __SCHED_H__
#define __SCHED_H__

#include "common.h"
#include "proc.h"

#include <stdint.h>

struct sched_entry {
	uint8_t pid;
	uint8_t len;
};

struct sched_entry schedule_get(uint64_t hartid, size_t i);

/**
 * @brief Initialize the scheduler.
 *
 * This function initializes the scheduler, which is responsible for managing
 * the execution of processes on the system. It sets up the necessary data
 * structures and configurations to support scheduling.
 */
void schedule_init(void);

/**
 * @brief Yield the current time slice for a given process.
 *
 * This function yields the current time slice for a given process. It marks
 * the process as being suspended and schedules the next available process
 * to run. When the yielded process is scheduled again, it will resume
 * execution from where it left off.
 *
 * @param proc Pointer to the process for which the time slice should be
 *             yielded.
 */
void schedule_yield(struct proc *proc);

/**
 * @brief Find the next process to schedule.
 *
 * This function finds the next process to schedule based on the current
 * state of the system.
 */
void schedule_next(void);

/// Delete scheduling at hartid, begin-end.
void schedule_delete(uint64_t hartid, uint64_t begin, uint64_t end);
/// Let pid run on hartid, begin-end.
void schedule_update(uint64_t hartid, uint64_t pid, uint64_t begin,
		     uint64_t end);

#endif /* __SCHED_H__ */
