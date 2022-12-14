#pragma once

#define UART ((volatile unsigned char*)0x10000000)

static inline void uart_init(void)
{
    UART[3] = 3;
    UART[2] = 1;
}

static inline int uart_putchar(char c)
{
    UART[0] = c;
    return c;
}

static inline int uart_getchar(void)
{
        while (!(UART[5] & 1));
        return UART[0];
}

static int uart_puts(const char *c) {
    int i = 0;
    while (c[i] != '\0') {
        uart_putchar(c[i]);
        i++;
    }
    return i;
}
