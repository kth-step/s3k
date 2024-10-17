#include "altc/altio.h"

#include <stdarg.h>
#include <stddef.h>

int alt_fputchar(FILE *f, char c);
int alt_fputuint(FILE *f, unsigned long long val);
int alt_fputhex(FILE *f, unsigned long long val);
int alt_fputstr(FILE *f, const char *s);
int alt_vfprintf(FILE *f, const char *fmt, va_list ap);
int alt_fprintf(FILE *f, const char *fmt, ...)

    int alt_putchar(char c);
int alt_putstr(const char *s);
int alt_putuint(unsigned long long val);
int alt_puthex(unsigned long long val);
int alt_vprintf(const char *fmt, va_list ap);
int alt_printf(const char *fmt, ...);

int alt_snputuint(char *buf, int size, unsigned long long val);
int alt_snputhex(char *buf, int size, unsigned long long val);
int alt_snvprintf(char *buf, int size, const char *fmt, va_list ap);
int alt_snprintf(char *buf, int size, const char *fmt, ...);
