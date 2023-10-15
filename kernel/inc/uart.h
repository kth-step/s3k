#pragma once
#include <stddef.h>

/* Initializes UART */
static inline void uart_init(void);
/* Puts one character in UART */
static inline int uart_putc(char c);
/* Gets one character from UART */
static inline int uart_getc(void);
/* Puts a string to UART */
static inline int uart_write(const char *s, int size);
/* Gets a NUL terminated string */
static inline int uart_read(char *s, int size);

#if defined(UART_NS16550A)
/* Driver for ns16550a UART. */
struct uart {
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

static volatile struct uart *UART0 = (volatile struct uart *)UART0_BASE_ADDR;

// Line status register flags
#define LSR_RX_READY 0x1  // Receive data ready
#define LSR_TX_READY 0x60 // Transmit data ready

void uart_init(void)
{
	UART0->lcr = 0x3;
	UART0->fcr = 0x1;
}

int uart_putc(char c)
{
	while (!(UART0->lsr & LSR_TX_READY))
		;
	UART0->thr = (unsigned char)c;
	return (unsigned char)c;
}

int uart_getc(void)
{
	while (!(UART0->lsr & LSR_RX_READY))
		;
	return UART0->rbr;
}

#elif defined(UART_SIFIVE)

/* Driver for SiFive's UART */

struct uart {
	int txdata; // Transmit data register
	int rxdata; // Receive data register
	int txctrl; // Transmit control register
	int rxctrl; // Receive control register
	int ie;	    // UART interrupt enable
	int ip;	    // UART interrupt pending
	int div;    // Baud rate divisor
};

static volatile struct uart *const UART0
    = (volatile struct uart *)UART0_BASE_ADDR;

// Bit masks for transmit and receive adata registers
#define TXDATA_FULL 0x80000000ul
#define RXDATA_EMPTY 0x80000000ul

// Control register flags for enabling transmission and reception
#define TXCTRL_TXEN 0x1ul
#define RXCTRL_RXEN 0x1ul

// Control register flags for setting stop bits
#define TXCTRL_NSTOP 0x2ul

void uart_init(void)
{
	UART0->txctrl = TXCTRL_TXEN; // Enable transmit data
	UART0->rxctrl = RXCTRL_RXEN; // Enable receive data
}

int uart_putc(char c)
{
	while (UART0->txdata & TXDATA_FULL) {
	}
	UART0->txdata = (unsigned char)c;
	return (unsigned char)c;
}

int uart_getc(void)
{
	int c;
	do {
		c = UART0->rxdata;
	} while (c & RXDATA_EMPTY);
	return c;
}
#else
#error "Missing UART"
#endif

int uart_write(const char *s, int size)
{
	if (s == NULL)
		return 0;

	int i = 0;
	for (i = 0; i < size && s[i] != '\0'; i++)
		uart_putc(s[i]);
	return i;
}

int uart_read(char *s, int size)
{
	if (s == NULL || size <= 0)
		return 0;

	int i;
	for (i = 0; i < size - 1; ++i) {
		s[i] = uart_getc();
		if (s[i] == '\n' || s[i] == '\r')
			break;
	}
	s[i] = '\0';
	return i;
}
