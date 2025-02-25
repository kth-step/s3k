#include "altc/altio.h"

#define UART_NS16550A(_base)                                               \
	(struct uart_ns16550a)                                             \
	{                                                                  \
		.fputchar = uart_ns16550a_fputchar,                        \
		.fgetchar = uart_ns16550a_fgetchar, .base = (void *)_base, \
	}

struct uart_ns16550a {
	int (*fputchar)(int c, ALTFILE *);
	int (*fgetchar)(ALTFILE *);
	void *base;
};

int uart_ns16550a_fputchar(int c, ALTFILE *f);
int uart_ns16550a_fgetchar(ALTFILE *f);
