#include <alt/io.h>

#define BUFFER(buf, size)                                                                                       \
	(struct buffer)                                                                                         \
	{                                                                                                       \
		._putc = buffer_putc, ._getc = buffer_getc, ._flush = buffer_flush, ._buf = buf, ._size = size, \
		._i = 0                                                                                         \
	}

struct buffer {
	int (*_putc)(FILE *, char);
	int (*_getc)(FILE *);
	void (*_flush)(FILE *);
	char *_buf;
	int _size;
	int _i;
};

int buffer_putc(FILE *f, char c)
{
	struct buffer *b = (struct buffer *)f;
	if (b->_i >= b->_size)
		return 0;
	return b->_buf[b->_i++] = c;
}

int buffer_getc(FILE *f)
{
	struct buffer *b = (struct buffer *)f;
	if (b->_i >= b->_size)
		return 0;
	return b->_buf[b->_i++];
}

void buffer_flush(FILE *f)
{
	// NOP
}

int alt_snputuint(char *buf, int size, unsigned long long val)
{
	struct buffer b = BUFFER(buf, size);
	alt_fputuint((FILE *)&b, val);
	if (b._i < b._size)
		buf[b._i] = '\0';
	return b._i;
}

int alt_snputhex(char *buf, int size, unsigned long long val)
{
	struct buffer b = BUFFER(buf, size);
	alt_fputhex((FILE *)&b, val);
	if (b._i < b._size)
		buf[b._i] = '\0';
	return b._i;
}

int alt_vsnprintf(char *buf, int size, const char *fmt, va_list ap)
{
	struct buffer b = BUFFER(buf, size);
	alt_vfprintf((FILE *)&b, fmt, ap);
	if (b._i < b._size)
		buf[b._i] = '\0';
	return b._i;
}

int alt_snprintf(char *buf, int size, const char *fmt, ...)
{
	struct buffer b = BUFFER(buf, size);
	va_list ap;
	va_start(ap, fmt);
	alt_vfprintf((FILE *)&b, fmt, ap);
	va_end(ap);
	if (b._i < b._size)
		buf[b._i] = '\0';
	return b._i;
}
