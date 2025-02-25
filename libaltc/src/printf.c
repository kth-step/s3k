#include "altc/altio.h"

#include <limits.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#define BUFFER_FILE(_buf, _size)                                          \
	(struct buffer_file)                                              \
	{                                                                 \
		.fputchar = buffer_fputchar, .buf = (char *)_buf, .i = 0, \
		.size = _size                                             \
	}

struct buffer_file {
	int (*fputchar)(int c, ALTFILE *);
	int (*fgetchar)(ALTFILE *);
	char *buf;
	size_t i;
	size_t size;
};

static int buffer_fputchar(int c, ALTFILE *f)
{
	struct buffer_file *bf = (struct buffer_file *)f;
	if (bf->i >= bf->size)
		return -1;
	bf->buf[bf->i++] = c;
	return 0;
}

int alt_printf(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int length = alt_vprintf(fmt, ap);
	va_end(ap);
	return length;
}

int alt_vprintf(const char *fmt, va_list ap)
{
	return alt_vfprintf(altout, fmt, ap);
}

int alt_sprintf(char *s, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int length = alt_vsprintf(s, fmt, ap);
	va_end(ap);
	return length;
}

int alt_vsprintf(char *s, const char *fmt, va_list ap)
{
	struct buffer_file bf = BUFFER_FILE(s, INT_MAX);
	alt_vfprintf((ALTFILE *)&bf, fmt, ap);
	return bf.i;
}

int alt_snprintf(char *s, size_t n, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int length = alt_vsnprintf(s, n, fmt, ap);
	va_end(ap);
	return length;
}

int alt_vsnprintf(char *s, size_t n, const char *fmt, va_list ap)
{
	struct buffer_file bf = BUFFER_FILE(s, n);
	alt_vfprintf((ALTFILE *)&bf, fmt, ap);
	return bf.i;
}

int alt_fprintf(ALTFILE *f, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int length = alt_vfprintf(f, fmt, ap);
	va_end(ap);
	return length;
}

void _print_hex(ALTFILE *f, unsigned long long x)
{
	if (!x) {
		alt_fputchar('0', f);
		return;
	}

	char hex[16];
	int i = 0;
	while (x) {
		unsigned int h = x % 16;
		if (h < 10)
			hex[i++] = '0' + h;
		else
			hex[i++] = 'A' + h - 10;
		x >>= 4;
	}
	while (i) {
		alt_fputchar(hex[--i], f);
	}
}

int alt_vfprintf(ALTFILE *f, const char *fmt, va_list ap)
{
	/* TODO */

	while (*fmt != '\0') {
		if (*fmt != '%') {
			alt_fputchar(*fmt, f);
			fmt++;
			continue;
		}
		fmt++;
		if (*fmt == '\0')
			break;
		switch (*fmt) {
		case 'x': {
			unsigned int x = va_arg(ap, unsigned int);
			_print_hex(f, x);
		} break;
		case 'X': {
			unsigned long long x = va_arg(ap, unsigned long long);
			_print_hex(f, x);
		} break;
		case 'c': {
			char c = va_arg(ap, int);
			alt_fputchar(c, f);
		} break;
		case 's': {
			char *s = va_arg(ap, char *);
			alt_fputstr(s, f);
		} break;
		default:
			alt_fputchar(*fmt, f);
		}
		fmt++;
	}
	alt_fputchar('\0', f);
	return 0;
}
