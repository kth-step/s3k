#pragma once

struct uart {
	int (*putchar)(int c, struct uart *uart);
	void *base;
};

static inline int uart_fputchar(int c, struct uart *uart)
{
	return uart->putchar(c, uart);
}

int uart_ns16550a_putchar(int c, struct uart *uart);
int uart_sifive_putchar(int c, struct uart *uart);

#define UART_NS16550A(_base)                  \
	((struct uart){                       \
	    .putchar = uart_ns16550a_putchar, \
	    .base = (void *)_base,            \
	})

#define UART_SIFIVE(_base)                  \
	((struct uart){                     \
	    .putchar = uart_sifive_putchar, \
	    .base = (void *)_base,          \
	})
