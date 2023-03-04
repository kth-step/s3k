#include "platform.h"

#include <sys/time.h>

static uint64_t timeout;
static uint64_t pmpcfg0;
static uint64_t pmpaddr[8];

uint64_t time_get(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000ull + tv.tv_usec;
}

uint64_t timeout_get(uint64_t i)
{
	return timeout;
}

void timeout_set(uint64_t i, uint64_t val)
{
	timeout = val;
}

uint64_t csrr_mhartid(void)
{
	return 0;
}

uint64_t csrr_mip(void)
{
	if (time_get() < timeout)
		return 0;
	return (1 << 7);
}

uint64_t csrr_pmpcfg(uint64_t i)
{
	return pmpcfg0;
}

void csrw_pmpcfg(uint64_t i, uint64_t val)
{
	pmpcfg0 = val;
}

uint64_t csrr_pmpaddr(uint64_t i)
{
	return pmpaddr[i];
}

void csrw_pmpaddr(uint64_t i, uint64_t val)
{
	pmpaddr[i] = val;
}
