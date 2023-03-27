/* See LICENSE file for copyright and license details. */
#include "init.h"

#include "altio.h"
#include "cnode.h"
#include "common.h"
#include "csr.h"
#include "init_caps.h"
#include "proc.h"
#include "schedule.h"
#include "ticket_lock.h"

void init_kernel(uint64_t payload)
{
	static struct ticket_lock lock;
	static volatile int done = 0;

	tl_acq(&lock);
	if (!done) {
		alt_printf("s3k(0x%X): Setting up.\n", csrr_mhartid());
		proc_init(payload);
		cnode_init(init_caps, ARRAY_SIZE(init_caps));
		schedule_init();
		__sync_synchronize();
		alt_printf("s3k(0x%X): Setup complete.\n", csrr_mhartid());
		done = 1;
	}
	alt_printf("s3k(0x%X): Running.\n", csrr_mhartid());
	tl_rel(&lock);
}
