#include <stdarg.h>
#include <stddef.h>

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

int alt_snprintf(char *restrict str, size_t size, const char *restrict fmt, ...)
{
	va_list args;
	char *s = str;
	const char *end = str + size - 1;
	va_start(args, fmt);
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
			s = write_char(s, end, va_arg(args, int));
			break;
		case 's':
			s = write_str(s, end, va_arg(args, char *));
			break;
		case 'x':
			s = write_hex(s, end, va_arg(args, unsigned int));
			break;
		case 'X':
			s = write_hex(s, end, va_arg(args, unsigned long long));
			break;
		case '\0':
			break;
		}
	}
	*s = '\0';
	va_end(args);
	return s - str;
}
