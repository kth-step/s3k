#ifndef __ALTIO_H__
#define __ALTIO_H__

#define UART_BASE ((volatile unsigned char *)0x10000000)

int alt_getchar(void);
int alt_putchar(char c);
int alt_putstr(const char *str);
int alt_puts(const char *str);
int alt_printf(const char *fmt, ...);

#endif /* __ALTIO_H__ */
