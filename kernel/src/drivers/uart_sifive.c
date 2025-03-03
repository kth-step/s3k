#include "drivers/uart.h"

// Bit masks for transmit and receive adata registers
#define TXDATA_FULL 0x80000000ul
#define RXDATA_EMPTY 0x80000000ul

// Control register flags for enabling transmission and reception
#define TXCTRL_TXEN 0x1ul
#define RXCTRL_RXEN 0x1ul

// Control register flags for setting stop bits
#define TXCTRL_NSTOP 0x2ul

struct uart_sifive_regs {
	int txdata; // Transmit data register
	int rxdata; // Receive data register
	int txctrl; // Transmit control register
	int rxctrl; // Receive control register
	int ie;	    // UART interrupt enable
	int ip;	    // UART interrupt pending
	int div;    // Baud rate divisor
};

int uart_sifive_putchar(int c, struct uart *f)
{
	volatile struct uart_sifive_regs *uart = f->base;
	while (uart->txdata & TXDATA_FULL) {
	}
	uart->txdata = (unsigned char)c;
	return (unsigned char)c;
}
