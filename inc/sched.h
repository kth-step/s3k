/* See LICENSE file for copyright and license details. */
#ifndef __SCHED_H__
#define __SCHED_H__
#include <stdint.h>

struct proc *schedule_yield(struct proc *proc);
struct proc *schedule_next(void);
void schedule_update(uint64_t hartid, uint64_t pid, uint64_t begin, uint64_t end);
#endif /* __SCHED_H__ */
