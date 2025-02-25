#include "kprintf.h"

#include <altc/altio.h>
#include <stdarg.h>

#if defined(NDEBUG) || !defined(VERBOSITY)
#undef VERBOSITY
#define VERBOSITY 0
#endif

#if NHART > 1
static int lock = 0;
#endif

void kprintf(int verb, const char *restrict fmt, ...)
{
	if (verb > VERBOSITY)
		return;

#if NHART > 1
	while (__atomic_fetch_or(&lock, 1, __ATOMIC_ACQUIRE))
		;
#endif
	va_list ap;
	va_start(ap, fmt);
	alt_vprintf(fmt, ap);
	va_end(ap);
#if NHART > 1
	__atomic_store_n(&lock, 0, __ATOMIC_RELEASE);
#endif
}
