#include "kprintf.h"

#include <altc/altio.h>
#include <stdarg.h>

#define BUF_SIZE 128

#if defined(NDEBUG) || !defined(VERBOSITY)
#undef VERBOSITY
#define VERBOSITY 0
#endif

#ifdef SMP
static int lock = 0;
#endif

void kprintf(int verb, const char *restrict fmt, ...)
{
	if (verb > VERBOSITY)
		return;
	char buf[BUF_SIZE];
	va_list ap;
	va_start(ap, fmt);
	alt_vsnprintf(buf, BUF_SIZE, fmt, ap);
	va_end(ap);

#ifdef SMP
	while (__atomic_fetch_or(&lock, 1, __ATOMIC_ACQUIRE))
		;
	alt_putstr(buf);
	__atomic_store_n(&lock, 0, __ATOMIC_RELEASE);
#else
	alt_putstr(buf);
#endif
}
