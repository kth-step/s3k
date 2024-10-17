#include "alt/io.h"

int alt_fputc(FILE *f, char c)
{
	return f->_putc(f, c);
}

int alt_fgetc(FILE *f)
{
	return f->_getc(f);
}

int alt_fwrite(FILE *f, const char *buf, int size)
{
	for (int i = 0; i < size; ++i)
		alt_fputc(f, buf[i]);
	return size;
}

int alt_fread(FILE *f, char *buf, int size)
{
	for (int i = 0; i < size; ++i)
		buf[i] = alt_fgetc(f);
	return size;
}

int alt_fputuint(FILE *f, unsigned long long val)
{
	int i = 0;
	char buf[32];
	do {
		int tmp = val % 10;
		buf[i++] = '0' + tmp;
		val /= 10;
	} while (val);

	for (int j = i - 1; j >= 0; --j)
		alt_fputc(f, buf[j]);

	return i;
}

int alt_fputhex(FILE *f, unsigned long long val)
{
	int i = 0;
	char buf[16];
	do {
		int tmp = val % 16;
		buf[i++] = tmp < 10 ? ('0' + tmp) : 'A' + (tmp - 10);
		val /= 16;
	} while (val);

	for (int j = i - 1; j >= 0; --j)
		alt_fputc(f, buf[j]);

	return i;
}

int alt_fputstr(FILE *f, const char *s)
{
	int i = 0;
	while (s[i] != '\0')
		alt_fputc(f, s[i++]);
	return i;
}

int alt_fgetstr(FILE *f, char *s)
{
	int i = 0;
	while (1) {
		s[i] = alt_fgetc(f);
		if (s[i] == '\0')
			break;
		i++;
	}
	return i;
}

int alt_vfprintf(FILE *f, const char *fmt, va_list ap)
{
	int i = 0;
	int j = 0;
	while (fmt[i] != '\0') {
		if (fmt[i] != '%') {
			alt_fputc(f, fmt[i]);
			i++;
			j++;
			continue;
		}

		switch (fmt[i + 1]) {
		case '%':
			alt_fputc(f, '%');
			j++;
			break;
		case 'c':
			alt_fputc(f, va_arg(ap, int));
			j++;
			break;
		case 's':
			j += alt_fputstr(f, va_arg(ap, char *));
			break;
		case 'x':
			j += alt_fputhex(f, va_arg(ap, unsigned int));
			break;
		case 'X':
			j += alt_fputhex(f, va_arg(ap, unsigned long));
			break;
		case 'u':
			j += alt_fputuint(f, va_arg(ap, unsigned int));
			break;
		case 'U':
			j += alt_fputuint(f, va_arg(ap, unsigned long));
			break;
		default:
			// Error: return negative number.
			return -1;
		}
		fmt += 2;
	}
	return j;
}

int alt_fprintf(FILE *f, const char *restrict fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int i = alt_vfprintf(f, fmt, ap);
	va_end(ap);
	return i;
}
