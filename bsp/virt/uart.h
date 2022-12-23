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
        while (!(UART[5] & 1))
                ;
        return UART[0];
}

static inline int uart_write(const char* buf, int nbytes)
{
        for (int i = 0; i < nbytes; i++)
                uart_putchar(buf[i]);
        return nbytes;
}

static inline int uart_puts(const char* s)
{
        int i = 0;
        while (s[i] != '\0') {
                uart_putchar(s[i]);
                i++;
        }
        uart_putchar('\r');
        uart_putchar('\n');
        return i;
}
