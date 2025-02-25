#include "altc/uart/sifive.h"
//
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

void uart_sifive_init(ALTFILE *f)
{
	volatile struct uart_sifive_regs *uart
	    = ((struct uart_sifive *)f)->base;
	uart->txctrl = TXCTRL_TXEN; // Enable transmit data
	uart->rxctrl = RXCTRL_RXEN; // Enable receive data
}

int uart_sifive_fputchar(int c, ALTFILE *f)
{
	volatile struct uart_sifive_regs *uart
	    = ((struct uart_sifive *)f)->base;
	while (uart->txdata & TXDATA_FULL) {
	}
	uart->txdata = (unsigned char)c;
	return (unsigned char)c;
}

int uart_sifive_fgetchar(ALTFILE *f)
{
	volatile struct uart_sifive_regs *uart
	    = ((struct uart_sifive *)f)->base;
	int c;
	do {
		c = uart->rxdata;
	} while (c & RXDATA_EMPTY);
	return c;
}
