#ifndef NDEBUG
#include "kassert.h"

#include "altio.h"

void kassert_failure(const char *file, uint64_t line, const char *expr)
{
	alt_printf("Assertion failed: %s, file %s, line 0x%X\n", expr, file,
		   line);
	while (1) {
	} // halt the system
}

#endif /* NDEBUG */
