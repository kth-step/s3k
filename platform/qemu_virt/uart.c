#include "serio/ns16550a.h"

extern void *__base_uart;

int serio_putchar(int c)
{
	return serio_ns16550a_putchar(c, &__base_uart);
}
