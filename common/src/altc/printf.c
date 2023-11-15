#include "altc/altio.h"

#include <stdarg.h>
#include <stddef.h>

#define ALT_PRINTF_BUF_SIZE 128

static char *write_dec(char *restrict dst, const char *end,
		       unsigned long long val)
{
	if (!val && dst != end) {
		*(dst++) = '0';
	} else if (dst != end) {
		int i = 0;
		char buf[24];
		while (val) {
			int tmp = val % 10;
			buf[i++] = '0' + tmp;
			val /= 10;
		}
		while (i > 0 && dst != end)
			*(dst++) = buf[--i];
	}
	return dst;
}

static char *write_hex(char *restrict dst, const char *end,
		       unsigned long long val)
{
	if (!val && dst != end) {
		*(dst++) = '0';
	} else if (dst != end) {
		int i = 0;
		char buf[16];
		while (val) {
			int tmp = val & 0xF;
			buf[i++] = tmp < 10 ? ('0' + tmp) : 'A' + (tmp - 10);
			val >>= 4;
		}
		while (i > 0 && dst != end)
			*(dst++) = buf[--i];
	}
	return dst;
}

static char *write_str(char *restrict dst, const char *end, char *restrict src)
{
	while (dst != end && *src != '\0')
		*(dst++) = *(src++);
	return dst;
}

static char *write_char(char *restrict dst, const char *end, char c)
{
	if (dst != end)
		*(dst++) = c;
	return dst;
}

int alt_vsnprintf(char *restrict str, size_t size, const char *restrict fmt,
		  va_list ap)
{
	char *s = str;
	const char *end = str + size - 1;
	while (*fmt != '\0' && s != end) {
		if (*(fmt++) != '%') {
			s = write_char(s, end, *(fmt - 1));
			continue;
		}
		switch (*(fmt++)) {
		case '%':
			s = write_char(s, end, '%');
			break;
		case 'c':
			s = write_char(s, end, va_arg(ap, int));
			break;
		case 's':
			s = write_str(s, end, va_arg(ap, char *));
			break;
		case 'x':
			s = write_hex(s, end, va_arg(ap, unsigned int));
			break;
		case 'X':
			s = write_hex(s, end, va_arg(ap, unsigned long long));
			break;
		case 'd':
			s = write_dec(s, end, va_arg(ap, unsigned int));
			break;
		case 'D':
			s = write_dec(s, end, va_arg(ap, unsigned long long));
			break;
		case '\0':
			break;
		}
	}
	*s = '\0';
	return s - str;
}

int alt_snprintf(char *restrict str, size_t size, const char *restrict fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int len = alt_vsnprintf(str, size, fmt, ap);
	va_end(ap);
	return len;
}

int alt_printf(const char *restrict fmt, ...)
{
	char buf[ALT_PRINTF_BUF_SIZE];
	va_list ap;
	va_start(ap, fmt);
	int len = alt_vsnprintf(buf, ALT_PRINTF_BUF_SIZE, fmt, ap);
	va_end(ap);
	alt_putstr(buf);
	return len;
}
