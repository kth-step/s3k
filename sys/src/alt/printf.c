#include "alt/io.h"

extern FILE *const _altout;
extern FILE *const _altin;

int alt_putc(char c)
{
	return alt_fputc(_altout, c);
}

int alt_getc(void)
{
	return alt_fgetc(_altin);
}

int alt_write(const char *buf, int size)
{
	return alt_fwrite(_altout, buf, size);
}

int alt_read(char *buf, int size)
{
	return alt_fread(_altin, buf, size);
}

int alt_putstr(const char *s)
{
	return alt_fputstr(_altout, s);
}

int alt_getstr(char *s)
{
	return alt_fgetstr(_altin, s);
}

int alt_putuint(unsigned long long u)
{
	return alt_fputuint(_altout, u);
}

int alt_puthex(unsigned long long h)
{
	return alt_fputhex(_altout, h);
}

int alt_vprintf(const char *restrict fmt, va_list ap)
{
	return alt_vfprintf(_altout, fmt, ap);
}

int alt_printf(const char *restrict fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int i = alt_vprintf(fmt, ap);
	va_end(ap);
	return i;
}
