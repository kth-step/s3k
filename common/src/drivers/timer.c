#include "drivers/timer.h"

extern volatile uint64_t _mtime[];
extern volatile uint64_t _mtimecmp[];

/** Get the RT clock */
uint64_t time_get(void)
{
	return _mtime[0];
}

/** Set the RT clock */
void time_set(uint64_t time)
{
	_mtime[0] = time;
}

/** Get the RT clock timeout for hart `hartid' */
uint64_t timer_get(uint64_t hartid)
{
	return _mtimecmp[hartid];
}

/** Set the RT clock timeout for hart `hartid' */
void timer_set(uint64_t hartid, uint64_t timeout)
{
	_mtimecmp[hartid] = timeout;
}
