#include "altc/altio.h"

#include <stdarg.h>
#include <stddef.h>
#include <limits.h>
#include <stdint.h>

#define BUFFER_FILE(_buf, _size) \
(struct buffer_file) { \
	.fputchar = buffer_fputchar, \
	.buf = (char*)_buf, \
 	.i = 0, \
 	.size = _size \
}

struct buffer_file {
	int (*fputchar)(int c, ALTFILE *);
	int (*fgetchar)(ALTFILE *);
	char *buf;
	size_t i;
	size_t size;
};

int buffer_fputchar(int c, ALTFILE *f)
{
	struct buffer_file *bf = (struct buffer_file*)f;
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

int alt_snprintf(char *s, size_t n, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int length = alt_vsnprintf(s, n, fmt, ap);
	va_end(ap);
	return length;
}

int alt_vsprintf(char *s, const char *fmt, va_list ap)
{
	struct buffer_file b = BUFFER_FILE(s, INT_MAX);
	return alt_vfprintf((ALTFILE*)&b, fmt, ap);
}

int alt_vsnprintf(char *s, size_t n, const char *fmt, va_list ap)
{
	struct buffer_file b = BUFFER_FILE(s, n);
	return alt_vfprintf((ALTFILE*)&b, fmt, ap);
}

int alt_fprintf(ALTFILE *f, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int length = alt_vfprintf(f, fmt, ap);
	va_end(ap);
	return length;
}

int alt_vfprintf(ALTFILE *f, const char *fmt, va_list ap)
{
	return 0;
}

/*

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
	n		break;
		case 'x':
			s = write_hex(s, end, va_arg(ap, unsigned int));
			break;
		case 'X':
			s = write_hex(s, end, va_arg(ap, unsigned long));
			break;
		case 'd':
			s = write_dec(s, end, va_arg(ap, unsigned int));
			break;
		case 'D':
			s = write_dec(s, end, va_arg(ap, unsigned long));
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
*/
