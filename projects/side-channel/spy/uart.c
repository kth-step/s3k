#include <stdio.h>

extern volatile int __uart_base[]; // UART base address

int __uart_putc(char c, FILE *f)
{
	(void)f;
	while (!(__uart_base[5] & 0x20)) {
	}
	__uart_base[0] = (unsigned char)c;
	return c;
}

int __uart_getc(FILE *f)
{
	return 0;
}

static FILE __stdio = FDEV_SETUP_STREAM(__uart_putc, __uart_getc, NULL, _FDEV_SETUP_RW);

FILE *const stdin = &__stdio;
__strong_reference(stdin, stdout);
__strong_reference(stdin, stderr);
