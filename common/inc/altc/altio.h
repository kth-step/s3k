#pragma once
#include <stddef.h>

int alt_getchar(void);
int alt_putchar(char c);
int alt_putstr(const char *str);
int alt_puts(const char *str);
int alt_gets(char *str);
int alt_printf(const char *fmt, ...);
int alt_snprintf(char *restrict s, size_t n, char *restrict fmt, ...);
