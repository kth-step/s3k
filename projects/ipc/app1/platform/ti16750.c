#include <stdio.h>

extern volatile int __uart_base[]; // UART base address

#ifndef UART_REG_STRIDE
#error "UART_REG_STRIDE must be defined (1 or 4 bytes) to set the register stride."
#endif

#if (UART_REG_STRIDE != 1) && (UART_REG_STRIDE != 4)
#error "Unsupported UART_REG_STRIDE. Use '1' for 1-byte stride or '4' for 4-byte stride."
#endif

/* TI16750 Register Structure Definition */

/* * Define a register slot that accounts for padding.
 * This ensures the data byte always sits at the start of the stride.
 */
struct __attribute__((packed)) uart_reg_t {
    volatile char val; // The actual 8-bit register data
    char __pad[UART_REG_STRIDE - 1]; // Padding to fulfill the stride
};

/* TI16750 Register Structure Definition with explicit padding */
struct __attribute__((packed)) uart_regs {
    /* Offset 0x00 */
    union {
        struct uart_reg_t rbr; // Receiver buffer register (read only)
        struct uart_reg_t thr; // Transmitter holding register (write only)
        struct uart_reg_t dll; // Divisor latch LSB (when LCR bit 7 = 1)
    };

    /* Offset 0x01 * UART_REG_STRIDE */
    union {
        struct uart_reg_t ier; // Interrupt enabler register
        struct uart_reg_t dlm; // Divisor latch MSB (when LCR bit 7 = 1)
    };

    /* Offset 0x02 * UART_REG_STRIDE */
    union {
        struct uart_reg_t iir; // Interrupt identification register (read only)
        struct uart_reg_t fcr; // FIFO control register (write only)
        struct uart_reg_t efr; // Enhanced feature register (when LCR = 0xBF)
    };

    /* Offset 0x03 * UART_REG_STRIDE */
    struct uart_reg_t lcr;     // Line control register

    /* Offset 0x04 * UART_REG_STRIDE */
    union {
        struct uart_reg_t mcr; // Modem control register
        struct uart_reg_t xon1; // XON1 character (when LCR = 0xBF)
    };

    /* Offset 0x05 * UART_REG_STRIDE */
    union {
        struct uart_reg_t lsr; // Line status register
        struct uart_reg_t xon2; // XON2 character (when LCR = 0xBF)
    };

    /* Offset 0x06 * UART_REG_STRIDE */
    union {
        struct uart_reg_t msr; // Modem status register
        struct uart_reg_t xoff1; // XOFF1 character (when LCR = 0xBF)
    };

    /* Offset 0x07 * UART_REG_STRIDE */
    union {
        struct uart_reg_t spr; // Scratch pad register
        struct uart_reg_t xoff2; // XOFF2 character (when LCR = 0xBF)
    };
};

int __uart_putc(char c, FILE *f)
{
	(void)f;
	volatile struct uart_regs *regs = (struct uart_regs *)__uart_base;
	while (!(regs->lsr.val & 0x20)) {
	}
	regs->thr.val = (unsigned char)c;
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
