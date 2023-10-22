#include "altc/altio.h"
#include "drivers/uart.h"
#include "plat/config.h"

#include <stdarg.h>

int alt_putchar(char c)
{
	uart_putc((void *)UART0_BASE_ADDR, c);
	return c;
}

int alt_putstr(const char *str)
{
	int i = 0;
	while (str[i] != '\0') {
		alt_putchar(str[i]);
		i++;
	}
	return i;
}

int alt_puts(const char *str)
{
	int i = alt_putstr(str);
	alt_putchar('\r');
	alt_putchar('\n');
	return i;
}
