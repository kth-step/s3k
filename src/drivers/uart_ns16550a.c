#include "drivers/uart.h"

// Line status register flags
#define LSR_RX_READY 0x1  // Receive data ready
#define LSR_TX_READY 0x60 // Transmit data ready

struct uart_ns16550a_regs {
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

int uart_ns16550a_putchar(int c, struct uart *f)
{
	volatile struct uart_ns16550a_regs *base = f->base;
	while (!(base->lsr & LSR_TX_READY))
		;
	base->thr = (unsigned char)c;
	return (unsigned char)c;
}
