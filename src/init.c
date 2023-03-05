/* See LICENSE file for copyright and license details. */
#include "init.h"

#include "cnode.h"
#include "common.h"
#include "lock.h"
#include "proc.h"
#include "schedule.h"

extern unsigned char _payload[];

static void init_proc()
{
	for (uint64_t pid = 0; pid < NPROC; ++pid) {
		processes[pid].pid = pid;
		processes[pid].state = PS_SUSPENDED;
	}
	processes[0].regs[REG_PC] = (uint64_t)_payload;
	processes[0].state = PS_READY;
}

static void init_caps()
{
	const union cap caps[] = INITIAL_CAPABILITIES;
	// Initialize cnode to get working root handle
	cnode_init();
	// Ger root handle
	cnode_handle_t root = cnode_get_root_handle();
	// Add capabilities to proc 0
	for (size_t i = 0; i < ARRAY_SIZE(caps); i++) {
		uint32_t handle = cnode_get_handle(0, i);
		cnode_insert(handle, caps[i], root);
	}
}

void init_kernel(uint64_t payload)
{
	static struct ticket_lock lock;
	static volatile int done = 0;

	tl_lock(&lock);
	if (!done) {
		init_proc();
		init_caps();
		schedule_init();
		__sync_synchronize();
		done = 1;
	}
	tl_unlock(&lock);
}
