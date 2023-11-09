#include "kprintf.h"

#include <altc/altio.h>
#include <stdarg.h>

#define BUF_SIZE 128

void kputstr(char *buf)
{
	static int lock = 0;
	while (__atomic_fetch_or(&lock, 1, __ATOMIC_ACQUIRE))
		;
	alt_putstr(buf);
	__atomic_store_n(&lock, 0, __ATOMIC_RELEASE);
}

void kprintf(char *restrict fmt, ...)
{
	char buf[BUF_SIZE];
	va_list ap;
	va_start(ap, fmt);
	alt_vsnprintf(buf, BUF_SIZE, fmt, ap);
	va_end(ap);
	kputstr(buf);
}
