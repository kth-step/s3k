#pragma once

#define UART ((volatile unsigned char*)0x10010008)

static inline void uart_init(void)
{
        volatile unsigned char* uart = UART;
        uart[3] = 3;
        uart[2] = 1;
}

static inline int uart_putchar(char c)
{
        volatile unsigned char* uart = UART;
        uart[0] = c;
        return c;
}

static inline int uart_getchar(void)
{
        volatile unsigned char* uart = UART;
        if (uart[5] & 1)
                return uart[0];
        return 0;
}

static int uart_puts(const char* c)
{
        int i = 0;
        while (c[i] != '\0') {
                uart_putchar(c[i]);
                i++;
        }
        return i;
}
