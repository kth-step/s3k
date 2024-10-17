#include "drivers/uart/ns16550a.h"

#define RBR 0 // Receiver buffer register
#define THR 0 // Transmitter holding register
#define IER 1 // Interrupt enable register
#define IIR 2 // Interrupt identification register
#define FCR 2 // FIFO control register
#define LCR 3 // Line control register
#define LSR 5 // Line status register

#define LSR_RX_READY 0x1  // Receive data ready
#define LSR_TX_READY 0x60 // Transmit data ready

void uart_ns16550a_init(FILE *f)
{
	// TODO:
	// regs[LCR] = 0x3;
	// regs[FCR] = 0x1;
}

int uart_ns16550a_putc(FILE *f, char c)
{
	// regs = pointer to memory mapped registers
	volatile char *regs = ((struct uart_ns16550a *)f)->_base;
	// Wait until transmit register is empty.
	while (!(regs[LSR] & LSR_TX_READY))
		;
	// Write char to transmit register
	regs[THR] = c;
	return c;
}

int uart_ns16550a_getc(FILE *f)
{
	// regs = pointer to memory mapped registers
	volatile char *regs = ((struct uart_ns16550a *)f)->_base;
	// Wait until receive register is ready.
	while (!(regs[LSR] & LSR_RX_READY))
		;
	// Read from receive register
	return regs[RBR];
}

void uart_ns16550a_flush(FILE *f)
{
	// NOP
}
