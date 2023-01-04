/* See LICENSE file for copyright and license details. */
#include "timer.h"

#define MTIME	    ((volatile uint64_t *)0x200bff8ull)
#define MTIMECMP(x) ((volatile uint64_t *)(0x2004000ull + ((x)*8)))

uint64_t timer_gettime(void)
{
	return *MTIME;
}

void timer_settime(uint64_t time)
{
	*MTIME = time;
}

uint64_t timer_gettimer(uint64_t hartid)
{
	return *MTIMECMP(hartid);
}

void timer_settimer(uint64_t hartid, uint64_t time)
{
	*MTIMECMP(hartid) = time;
}

