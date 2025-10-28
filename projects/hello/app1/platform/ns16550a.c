#include <stdio.h>

extern volatile int __uart_base[]; // UART base address

#define LSR_RX_READY 0x1  // Receive data ready
#define LSR_TX_READY 0x60 // Transmit data ready

struct uart_regs {
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

int __uart_putc(char c, FILE *f)
{
	(void)f;
	volatile struct uart_regs *regs = (struct uart_regs*)__uart_base;
	while (!(regs->lsr & LSR_TX_READY))
		;
	regs->thr = (unsigned char)c;
	return (unsigned char)c;
}

int __uart_getc(FILE *f)
{
	(void)f;
	return 0;
}

static FILE __stdio = FDEV_SETUP_STREAM(__uart_putc, __uart_getc, NULL, _FDEV_SETUP_RW);

FILE *const stdin = &__stdio;
__strong_reference(stdin, stdout);
__strong_reference(stdin, stderr);
