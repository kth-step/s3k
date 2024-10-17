#include "drivers/uart/sifive.h"

#define TXDATA 0 // Transmit data register
#define RXDATA 1 // Receive data register
#define TXCTRL 2 // Transmit control register
#define RXCTRL 3 // Receive control register
#define IE 4	 // UART interrupt enable
#define IP 5	 // UART interrupt pending
#define DIV 6	 // Baud rate divisor

// Bit masks for transmit and receive registers
#define TXDATA_FULL 0x80000000ul
#define RXDATA_EMPTY 0x80000000ul

// Control register flags for enabling transmission and reception
#define TXCTRL_TXEN 0x1ul
#define RXCTRL_RXEN 0x1ul

// Control register flags for setting stop bits
#define TXCTRL_NSTOP 0x2ul

void uart_sifive_init(FILE *f)
{
	// TODO
	// UART0->txctrl = TXCTRL_TXEN; // Enable transmit data
	// UART0->rxctrl = RXCTRL_RXEN; // Enable receive data
	// UART0->div = TLCLK / TARGET_BAUD_RATE;
}

int uart_sifive_putc(FILE *, char c)
{
	volatile unsigned int *regs = ((struct uart_sifive *)f)->base_address;
	while (regs[TXDATA] & TXDATA_FULL) {
	}
	regs[TXDATA] = c;
	return (unsigned char)c;
}

int uart_sifive_getc(FILE *f)
{
	volatile unsigned int *regs = ((struct uart_sifive *)f)->base_address;
	int c;
	while ((c = regs[RXDATA]) & RXDATA_EMPTY)
		;
	return c;
}

void uart_sifive_flush(FILE *f)
{
	// NOP
}
