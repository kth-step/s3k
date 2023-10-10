#include "drivers/uart.h"

#define LINE_STATUS_DATA_READY 0x1
#define FIFO_CONTROL_REGISTER 0x2
#define LINE_CONTROL_REGISTER 0x3
#define LINE_STATUS_REGISTER 0x5

extern volatile char _uart[];

void uart_init(void)
{
	_uart[LINE_CONTROL_REGISTER] = 0x3;
	_uart[FIFO_CONTROL_REGISTER] = 0x1;
}

int uart_putc(char c)
{
	_uart[0] = c;
	return c;
}

int uart_getc(void)
{
	while (!(_uart[LINE_STATUS_REGISTER] & LINE_STATUS_DATA_READY))
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
