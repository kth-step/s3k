#include "timer.h"

extern volatile uint64_t _mtime[];
extern volatile uint64_t _mtimecmp[];

uint64_t time_get(void)
{
	return _mtime[0];
}

void time_set(uint64_t time)
{
	_mtime[0] = time;
}

uint64_t timeout_get(uint64_t i)
{
	return _mtimecmp[i];
}

void timeout_set(uint64_t i, uint64_t val)
{
	_mtimecmp[i] = val;
}
