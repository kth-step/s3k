#pragma once
#include "alt/io.h"

#define UART_NS16550A(base)                                               \
	(struct uart_ns16550a)                                            \
	{                                                                 \
		._putc = uart_ns16550a_putc, ._getc = uart_ns16550a_getc, \
		._flush = uart_ns16550a_flush, ._base = (char *)base,     \
	}

typedef struct uart_ns16550a {
	int (*_putc)(FILE *f, char c);
	int (*_getc)(FILE *f);
	void (*_flush)(FILE *f);
	char *_base;
} uart_ns16550a_t;

int uart_ns16550a_putc(FILE *f, char c);
int uart_ns16550a_getc(FILE *f);
void uart_ns16550a_flush(FILE *f);
