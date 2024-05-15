#include <stdint.h>

struct uart {
	union {
		uint32_t rbr;
		uint32_t thr;
		uint32_t dlab_lsb;
	};
	union {
		uint32_t interrupt_enable;
		uint32_t dlab_msb;
	};
	uint32_t interrupt_ident;
	uint32_t fifo_control;
	uint32_t line_control;
	uint32_t modem_control;
	uint32_t line_status;
	uint32_t modem_status;
};

void uart_init(void *base, uint64_t freq, uint64_t baud)
{
	volatile struct uart *uart = base;
	uint64_t divisor = freq / (baud << 4);
	uint8_t dlo = (uint8_t)(divisor);
	uint8_t dhi = (uint8_t)(divisor >> 8);
	*((char*)&uart->interrupt_enable) = 0x00;
	*((char*)&uart->line_control) = 0x80;
	*((char*)&uart->dlab_lsb) = dlo;
	*((char*)&uart->dlab_msb) = dhi;
	*((char*)&uart->line_control) = 0x03;
	*((char*)&uart->fifo_control) = 0xC7;
	*((char*)&uart->modem_control) = 0x20;
}

int uart_putc(void *base, char c)
{
	volatile struct uart *uart = base;
	while (!(uart->line_status & 0x20))
		;
	uart->thr = (unsigned char)c;
	return (unsigned char)c;
}

int uart_getc(void *base)
{
	return 0;
}

