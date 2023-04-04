#include "drivers/uart.h"

extern volatile char _uart[];

void uart_init(void)
{
	/** TODO: Proper init code for uart */
}

int uart_putc(char c)
{
	_uart[0] = c;
	return c;
}

int uart_getc(void)
{
	while (!(_uart[5] & 0x1))
		;
	return _uart[0];
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
