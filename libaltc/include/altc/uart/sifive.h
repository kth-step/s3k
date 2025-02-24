#include "altc/altio.h"

#define UART_SIFIVE(_base) \
(struct uart_sifive) { \
	.fputchar = uart_sifive_fputchar, \
	.fgetchar = uart_sifive_fgetchar, \
	.base = (void*)_base, \
}

struct uart_sifive {
	int (*fputchar)(int c, ALTFILE *);
	int (*fgetchar)(ALTFILE *);
	void *base;
};

int uart_sifive_fputchar(int c, ALTFILE *f);
int uart_sifive_fgetchar(ALTFILE *f);
