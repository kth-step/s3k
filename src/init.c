/* See LICENSE file for copyright and license details. */
#include "init.h"

#include "cnode.h"
#include "common.h"
#include "init_caps.h"
#include "lock.h"
#include "proc.h"
#include "schedule.h"

extern unsigned char _payload[];

void init_kernel(uint64_t payload)
{
	static struct ticket_lock lock;
	static volatile int done = 0;

	tl_lock(&lock);
	if (!done) {
		proc_init(payload);
		cnode_init(init_caps, ARRAY_SIZE(init_caps));
		schedule_init();
		__sync_synchronize();
		done = 1;
	}
	tl_unlock(&lock);
}
