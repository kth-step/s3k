#include "serio/uart/ti16750.h"

extern void *__base_uart;

int serio_putchar(int c)
{
	return serio_ti16750a_putchar(c, &__base_uart);
}
