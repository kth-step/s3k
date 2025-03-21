#include "serio/ti16750.h"

extern void *__base_uart;

int serio_putchar(int c)
{
	return serio_ti16750_putchar(c, &__base_uart);
}
