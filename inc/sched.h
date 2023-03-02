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

#include <stdint.h>

#include "common.h"

/// Yield the current time slice.
struct proc *sched_yield(struct proc *proc);
/// Find the next process to schedule.
struct proc *sched_next(void);
/// Initialize scheduler.
void sched_init(void);

/// Delete scheduling at hartid, begin-end.
void sched_delete(uint64_t hartid, uint64_t begin, uint64_t end);
/// Let pid run on hartid, begin-end.
void sched_update(uint64_t hartid, uint64_t pid, uint64_t begin, uint64_t end);

#endif /* __SCHED_H__ */
