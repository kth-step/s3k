#include "drivers/uart.h"

// Line status register flags
#define LSR_RX_READY 0x1  // Receive data ready
#define LSR_TX_READY 0x60 // Transmit data ready

struct uart {
	union {
		char rbr; // Receiver buffer register (read only)
		char thr; // Transmitter holding register (write only)
	};

	char ier; // Interrupt enabler register

	union {
		char iir; // Interrupt identification register (read only)
		char fcr; // FIFO control register (write only)
	};

	char lcr; // Line control register
	char __padding;
	char lsr; // Line status register
};

void uart_init(void *base)
{
	volatile struct uart *uart = base;
	uart->lcr = 0x3;
	uart->fcr = 0x1;
}

int uart_putc(void *base, char c)
{
	volatile struct uart *uart = base;
	while (!(uart->lsr & LSR_TX_READY))
		;
	uart->thr = (unsigned char)c;
	return (unsigned char)c;
}

int uart_getc(void *base)
{
	volatile struct uart *uart = base;
	while (!(uart->lsr & LSR_RX_READY))
		;
	return uart->rbr;
}
