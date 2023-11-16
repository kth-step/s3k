#include "kprintf.h"

#include <altc/altio.h>
#include <stdarg.h>

#define BUF_SIZE 128

#if defined(NDEBUG) || !defined(VERBOSITY)
#undef VERBOSITY
#define VERBOSITY 0
#endif

void kprintf(int verbosity, char *restrict fmt, ...)
{
	static int lock = 0;

	if (verbosity > VERBOSITY)
		return;

	char buf[BUF_SIZE];
	va_list ap;
	va_start(ap, fmt);
	alt_vsnprintf(buf, BUF_SIZE, fmt, ap);
	va_end(ap);

	while (__atomic_fetch_or(&lock, 1, __ATOMIC_ACQUIRE))
		;
	alt_putstr(buf);
	__atomic_store_n(&lock, 0, __ATOMIC_RELEASE);
}
