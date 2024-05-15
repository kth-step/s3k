#pragma once

#include <stdint.h>

/* Initializes UART */
void uart_init(void *base, uint64_t freq, uint64_t baud);

/* Puts one character in UART */
int uart_putc(void *base, char c);

/* Gets one character from UART */
int uart_getc(void *base);
