#include "altc/altio.h"

#include <stdarg.h>

int alt_printf(const char *fmt, ...)
{
	static const char hex[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
				     '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
	unsigned long long x;
	int len;
	va_list args;
	va_start(args, fmt);
	len = 0;

	while (*fmt != '\0') {
		if (*fmt++ != '%') {
			alt_putchar(*(fmt - 1));
			len++;
			continue;
		}
		switch (*fmt++) {
		case 'c':
			alt_putchar((char)va_arg(args, int));
			len++;
			break;
		case 's':
			len += alt_putstr(va_arg(args, char *));
			break;
		case 'x':
			x = va_arg(args, unsigned int);
			if (!x) {
				alt_putchar('0');
				len++;
				break;
			}
			for (int i = 28; i >= 0; i -= 4) {
				if (x >> i) {
					alt_putchar(hex[(x >> i) & 0xF]);
					len++;
				}
			}
			break;
		case 'X':
			x = va_arg(args, unsigned long long);
			if (!x) {
				alt_putchar('0');
				len++;
				break;
			}
			for (int i = 60; i >= 0; i -= 4) {
				if (x >> i) {
					alt_putchar(hex[(x >> i) & 0xF]);
					len++;
				}
			}
			break;
		case '%':
			alt_putchar('%');
			len++;
			break;
		case '\0':
			break;
		}
	}
	va_end(args);
	return len;
}
