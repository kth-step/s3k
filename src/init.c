/* See LICENSE file for copyright and license details. */
#include "init.h"

#include "bsp/init_caps.h"
#include "cnode.h"
#include "common.h"
#include "lock.h"
#include "proc.h"
#include "sched.h"

void init_kernel(uint64_t payload)
{
	static struct ticket_lock lock;
	static volatile int done = 0;

	tl_lock(&lock);
	if (!done) {
		for (uint64_t pid = 0; pid < NPROC; ++pid) {
			processes[pid].pid = pid;
			processes[pid].state = PS_SUSPENDED;
		}
		processes[0].regs.pc = payload;
		processes[0].state = PS_READY;
		init_caps();
		sched_init();
		__sync_synchronize();
		done = 1;
	}
	tl_unlock(&lock);
}
