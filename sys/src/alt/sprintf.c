#include <alt/io.h>

#define BUFFER(buf)                                          \
	(struct buffer)                                      \
	{                                                    \
		._putc = buffer_putc, ._getc = buffer_getc,  \
		._flush = buffer_flush, ._buf = buf, ._i = 0 \
	}

struct buffer {
	int (*_putc)(FILE *, char);
	int (*_getc)(FILE *);
	void (*_flush)(FILE *);
	char *_buf;
	int _i;
};

int buffer_putc(FILE *f, char c)
{
	struct buffer *b = (struct buffer *)f;
	return b->_buf[b->_i++] = c;
}

int buffer_getc(FILE *f)
{
	struct buffer *b = (struct buffer *)f;
	return b->_buf[b->_i++];
}

void buffer_flush(FILE *f)
{
	// NOP
}

int alt_sputuint(char *buf, unsigned long long val)
{
	struct buffer b = BUFFER(buf);
	alt_fputuint((FILE *)&b, val);
	buf[b._i] = '\0';
	return b._i;
}

int alt_sputhex(char *buf, unsigned long long val)
{
	struct buffer b = BUFFER(buf);
	alt_fputhex((FILE *)&b, val);
	buf[b._i] = '\0';
	return b._i;
}

int alt_vsprintf(char *buf, const char *fmt, va_list ap)
{
	struct buffer b = BUFFER(buf);
	alt_vfprintf((FILE *)&b, fmt, ap);
	buf[b._i] = '\0';
	return b._i;
}

int alt_sprintf(char *buf, const char *fmt, ...)
{
	struct buffer b = BUFFER(buf);
	va_list ap;
	va_start(ap, fmt);
	alt_vfprintf((FILE *)&b, fmt, ap);
	va_end(ap);
	buf[b._i] = '\0';
	return b._i;
}
