#pragma once

/** Enable serial port. */
void uart_init(void);

/** Put char on serial port. */
int uart_putc(char c);

/** Get char from serial port. */
int uart_getc(void);

/** Puts a string on serial port */
int uart_puts(const char *s);

/** Gets a string from serial port */
char *uart_gets(char *s);
