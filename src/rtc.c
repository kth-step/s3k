#include "rtc.h"

// Define the base address of the CLINT peripheral
extern uint64_t __base_clint[];

// Define offsets for the mtime and mtimecmp registers
#define CLINT_MTIME_OFFSET 0xBFF8
#define CLINT_MTIMECMP_OFFSET 0x4000

// Calculate the addresses of the mtime and mtimecmp registers
static volatile uint64_t *const mtime
    = (volatile uint64_t *)((uintptr_t)__base_clint + CLINT_MTIME_OFFSET);
static volatile uint64_t *const mtimecmp
    = (volatile uint64_t *)((uintptr_t)__base_clint + CLINT_MTIMECMP_OFFSET);

uint64_t rtc_time_get(void)
{
	return mtime[0];
}

void rtc_time_set(uint64_t time)
{
	mtime[0] = time;
}

uint64_t rtc_timeout_get(uint64_t hartid)
{
	return mtimecmp[hartid];
}

void rtc_timeout_set(uint64_t hartid, uint64_t timeout)
{
	mtimecmp[hartid] = timeout;
}
