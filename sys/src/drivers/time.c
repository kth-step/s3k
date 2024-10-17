#include "drivers/time.h"

extern volatile uint64_t _mtime[];    // Set in linker script.
extern volatile uint64_t _mtimecmp[]; // Set in linker script.

uint64_t time_get(void)
{
	return _mtime[0];
}

void time_set(uint64_t val)
{
	_mtime[0] = val;
}

uint64_t timeout_get(uintptr_t hartid)
{
	return _mtimecmp[hartid];
}

void timeout_set(uintptr_t hartid, uint64_t val)
{
	_mtimecmp[hartid] = val;
}
