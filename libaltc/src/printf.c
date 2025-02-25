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

int alt_vfprintf(ALTFILE *f, const char *fmt, va_list ap)
{
	/* TODO */
	return 0;
}
