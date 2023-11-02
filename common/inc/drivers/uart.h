#pragma once

/* Initializes UART */
void uart_init(void *base);

/* Puts one character in UART */
int uart_putc(void *base, char c);

/* Gets one character from UART */
int uart_getc(void *base);
