#pragma once

#include <stdint.h>

/** Get the RT clock */
static inline uint64_t time_get(void);

/** Set the RT clock */
static inline void time_set(uint64_t time);

/** Get the RT clock timeout for hart `hartid' */
static inline uint64_t timer_get(uint64_t hartid);

/** Set the RT clock timeout for hart `hartid' */
static inline void timer_set(uint64_t hartid, uint64_t timeout);

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

uint64_t timer_get(uint64_t hartid)
{
	return MTIMECMP[hartid];
}

void timer_set(uint64_t hartid, uint64_t timeout)
{
	MTIMECMP[hartid] = timeout;
}
