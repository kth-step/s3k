#include "time.h"

#define MTIME_BASE_ADDR 0x200bff8ull
#define MTIMECMP_BASE_ADDR 0x2004000ull

static volatile uint64_t *const MTIME = (uint64_t *)MTIME_BASE_ADDR;
static volatile uint64_t *const MTIMECMP = (uint64_t *)MTIMECMP_BASE_ADDR;

uint64_t time_get(void)
{
	return MTIME[0];
}

void time_set(uint64_t time)
{
	MTIME[0] = time;
}

uint64_t timeout_get(uint64_t hartid)
{
	return MTIMECMP[hartid];
}

void timeout_set(uint64_t hartid, uint64_t timeout)
{
	MTIMECMP[hartid] = timeout;
}
