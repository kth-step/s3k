#include "altc/altio.h"
#include "drivers/uart.h"
#include "plat/config.h"

#include <stdarg.h>

int alt_getchar(void)
{
	return uart_getc((void *)UART0_BASE_ADDR);
}

int alt_gets(char *src)
{
	int i = 0;
	while (1) {
		src[i] = alt_getchar();
		if (src[i] == '\n' || src[i] == '\0')
			break;
		i++;
	}
	src[i] = '\0';
	return i;
}
