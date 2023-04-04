#include "drivers/uart.h"

extern volatile unsigned long long _uart[];

void uart_init(void)
{
	_uart[2] = 1;
	_uart[3] = 1;
}

int uart_putc(char c)
{
	while (_uart[0] < 0)
		;
	_uart[0] = c;
	return c;
}

int uart_getc(void)
{
	int c;
	while ((c = _uart[1]) < 0)
		;
	return c;
}

int uart_puts(const char *s)
{
	int i = 0;
	while (s[i] != '\0')
		uart_putc(s[i++]);
	uart_putc('\n');
	return i + 1;
}

char *uart_gets(char *s)
{
	while ((*s = (char)uart_getc()) != '\0')
		;
	return s;
}
