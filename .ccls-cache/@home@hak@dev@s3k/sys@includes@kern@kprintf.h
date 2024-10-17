#pragma once

#include "alt/io.h"

static inline void kprintf(int verb, const char *restrict fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	alt_printf(fmt, ap);
	va_end(ap);
}
