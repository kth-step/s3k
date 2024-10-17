#pragma once
#include "alt/io.h"

#define UART_SIFIVE(base_address)                                     \
	(struct uart_sifive)                                          \
	{                                                             \
		._putc = uart_sifive_putc, ._getc = uart_sifive_getc, \
		._flush = uart_sifive_flush,                          \
		.base_address = (int *)base_address,                  \
	}

struct uart_sifive {
	int (*_putc)(FILE *f, char c);
	int (*_getc)(FILE *f);
	void (*_flush)(FILE *f);
	int *base_address;
};

int uart_sifive_putc(FILE *f, char c);
int uart_sifive_getc(FILE *f);
void uart_sifive_flush(FILE *f);
