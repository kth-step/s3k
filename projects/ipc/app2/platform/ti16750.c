#include <stdio.h>

extern volatile int __uart_base[]; // UART base address

#ifndef UART_REG_STRIDE
#error "UART_REG_STRIDE must be defined (1 or 4)."
#endif

#if (UART_REG_STRIDE != 1) && (UART_REG_STRIDE != 4)
#error "Unsupported UART_REG_STRIDE. Use 1 or 4."
#endif

struct __attribute__((packed)) uart_reg_t {
	volatile char val;
	char __pad[UART_REG_STRIDE - 1];
};

struct __attribute__((packed)) uart_regs {
	union {
		struct uart_reg_t rbr;
		struct uart_reg_t thr;
		struct uart_reg_t dll;
	};

	union {
		struct uart_reg_t ier;
		struct uart_reg_t dlm;
	};

	union {
		struct uart_reg_t iir;
		struct uart_reg_t fcr;
		struct uart_reg_t efr;
	};

	struct uart_reg_t lcr;

	union {
		struct uart_reg_t mcr;
		struct uart_reg_t xon1;
	};

	union {
		struct uart_reg_t lsr;
		struct uart_reg_t xon2;
	};

	union {
		struct uart_reg_t msr;
		struct uart_reg_t xoff1;
	};

	union {
		struct uart_reg_t spr;
		struct uart_reg_t xoff2;
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
