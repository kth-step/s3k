#include "kprintf.h"

#include "drivers/uart.h"

#include <stdarg.h>

extern struct uart *const kout;

static void _putchar(char c, struct uart *uart)
{
	uart->putchar(c, uart);
}

static void _putstr(char *s, struct uart *f)
{
	while (*s != '\0') {
		_putchar(*s++, f);
	}
}

static void _puthex(unsigned long long x, struct uart *f)
{
	if (!x) {
		_putchar('0', f);
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
		_putchar(hex[--i], f);
	}
}

static void _kprintf(const char *fmt, va_list ap, struct uart *f)
{
	while (*fmt != '\0') {
		if (*fmt != '%') {
			_putchar(*fmt, f);
			fmt++;
			continue;
		}
		fmt++;
		if (*fmt == '\0')
			break;
		switch (*fmt) {
		case 'x': {
			unsigned int x = va_arg(ap, unsigned int);
			_puthex(x, f);
		} break;
		case 'X': {
			unsigned long long x = va_arg(ap, unsigned long long);
			_puthex(x, f);
		} break;
		case 'c': {
			char c = va_arg(ap, int);
			_putchar(c, f);
		} break;
		case 's': {
			char *s = va_arg(ap, char *);
			_putstr(s, f);
		} break;
		default:
			_putchar(*fmt, f);
		}
		fmt++;
	}
}

void kprintf(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	_kprintf(fmt, ap, kout);
	va_end(ap);
}
