#include "serio/ti16750.h"

extern void *__base_uart;

int serio_putchar(int c)
{
	if (c == '\n')
		serio_ti16750_putchar('\r', &__base_uart);
	return serio_ti16750_putchar(c, &__base_uart);
}
