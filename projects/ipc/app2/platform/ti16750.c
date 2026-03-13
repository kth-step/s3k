#include <stdio.h>

extern volatile int __uart_base[]; // UART base address
/* TI16750 Register Structure Definition */
struct uart_regs {
    union {
        char rbr; // Receiver buffer register (read only)
        char thr; // Transmitter holding register (write only)
        char dll; // Divisor latch LSB (when LCR bit 7 = 1)
    };

    union {
        char ier; // Interrupt enabler register
        char dlm; // Divisor latch MSB (when LCR bit 7 = 1)
    };

    union {
        char iir; // Interrupt identification register (read only)
        char fcr; // FIFO control register (write only)
        char efr; // Enhanced feature register (when LCR = 0xBF)
    };

    char lcr;     // Line control register

    union {
        char mcr; // Modem control register
        char xon1; // XON1 character (when LCR = 0xBF)
    };

    union {
        char lsr; // Line status register
        char xon2; // XON2 character (when LCR = 0xBF)
    };

    union {
        char msr; // Modem status register
        char xoff1; // XOFF1 character (when LCR = 0xBF)
    };

    union {
        char spr; // Scratch pad register
        char xoff2; // XOFF2 character (when LCR = 0xBF)
    };
};

int __uart_putc(char c, FILE *f)
{
	(void)f;
	volatile struct uart_regs *regs = (struct uart_regs *)__uart_base;
	while (!(regs->lsr & 0x20)) {
	}
	regs->thr = (unsigned char)c;
	return c;
}

int __uart_getc(FILE *f)
{
	return 0;
}

static FILE __stdio = FDEV_SETUP_STREAM(__uart_putc, __uart_getc, NULL, _FDEV_SETUP_RW);

FILE *const stdin = &__stdio;
__strong_reference(stdin, stdout);
__strong_reference(stdin, stderr);


