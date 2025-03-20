#include "kprintf.h"

#include "serio.h"

void kprintf(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	serio_vprintf(fmt, ap);
	va_end(ap);
}
