#include "drivers/time.h"

#include "plat/config.h"
static volatile s3k_addr_t *const MTIME = (s3k_addr_t *)MTIME_BASE_ADDR;
static volatile s3k_addr_t *const MTIMECMP = (s3k_addr_t *)MTIMECMP_BASE_ADDR;

s3k_time_t time_get(void)
{
	return MTIME[0];
}

void time_set(s3k_time_t time)
{
	MTIME[0] = time;
}

s3k_time_t timeout_get(s3k_hart_t hartid)
{
	return MTIMECMP[hartid];
}

void timeout_set(s3k_hart_t hartid, s3k_time_t timeout)
{
	MTIMECMP[hartid] = timeout;
}
