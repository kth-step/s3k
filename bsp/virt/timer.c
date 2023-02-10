/* See LICENSE file for copyright and license details. */
#include "bsp/timer.h"

#define MTIME	    ((volatile uint64_t *)0x200bff8ull)
#define MTIMECMP(x) ((volatile uint64_t *)(0x2004000ull + ((x)*8)))

uint64_t time_get(void)
{
	return *MTIME;
}

void time_set(uint64_t time)
{
	*MTIME = time;
}

uint64_t timeout_get(uint64_t hartid)
{
	return *MTIMECMP(hartid);
}

void timeout_set(uint64_t hartid, uint64_t time)
{
	*MTIMECMP(hartid) = time;
}

