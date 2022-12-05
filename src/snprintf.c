// See LICENSE file for copyright and license details.
#include "snprintf.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifndef NDEBUG
static inline char* outputStr(char* buf, char* ebuf, const char* s)
{
        while (buf != ebuf && *s != '\0') {
                *buf++ = *s++;
        }
        return buf;
}

static inline char* output64(char* buf, char* ebuf, long long n, int sig, int base, char padding, int padd_width)
{
        char buff[32];
        buff[31] = '\0';
        unsigned long long i = n;
        int neg = 0;
        if (n < 0 && sig) {
                padd_width--;
                neg = 1;
                i = -n;
        }
        char* c = &buff[31];
        do {
                unsigned long long j = i % base;
                if (j >= 10)
                        *--c = 'a' + j - 10;
                else
                        *--c = '0' + j;
                i /= base;
                padd_width--;
        } while (i != 0);
        while (padd_width > 0) {
                *--c = padding;
                padd_width--;
        }
        if (neg) {
                *--c = '-';
        }
        return outputStr(buf, ebuf, c);
}

static inline char* output32(char* buf, char* ebuf, int n, bool sig, int base, char padding, int padd_width)
{
        char buff[16];
        buff[15] = '\0';
        unsigned int i = n;
        bool neg = 0;
        if (n < 0 && sig) {
                padd_width--;
                neg = 1;
                i = -n;
        }
        char* c = &buff[15];
        do {
                unsigned int j = i % base;
                if (j >= 10)
                        *--c = 'a' + j - 10;
                else
                        *--c = '0' + j;
                i /= base;
                padd_width--;
        } while (i != 0);
        while (padd_width > 0) {
                *--c = padding;
                padd_width--;
        }
        if (neg) {
                *--c = '-';
        }
        return outputStr(buf, ebuf, c);
}

int vsnprintf(char* buf, size_t n, const char* format, va_list args)
{
        if (n == 0)
                return 0;
        char* ebuf = buf + n - 1;
        for (const char* f = format; *f != '\0' && buf != ebuf; ++f) {
                if (*f != '%') {
                        *(buf++) = *f;
                        continue;
                }
                f++;
                char padding = ' ';
                int padd_width = 0;
                if (*f == '0') {
                        padding = '0';
                        f++;
                }
                while (*f >= '0' && *f <= '9') {
                        padd_width *= 10;
                        padd_width += *f - '0';
                        f++;
                }
                if (*f == 'd') {
                        buf = output32(buf, ebuf, va_arg(args, int), true, 10, padding, padd_width);
                } else if (*f == 'u') {
                        buf = output32(buf, ebuf, va_arg(args, int), false, 10, padding, padd_width);
                } else if (*f == 'x') {
                        buf = output32(buf, ebuf, va_arg(args, int), false, 1, padding, padd_width);
                } else if (*f == 's') {
                        buf = outputStr(buf, ebuf, va_arg(args, char*));
                } else if (*f == 'c') {
                        *(buf++) = va_arg(args, int);
                } else if (*f == '%') {
                        *(buf++) = '%';
                } else if (*f == 'l') {
                        f++;
                        if (*f == 'd') {
                                buf = output64(buf, ebuf, va_arg(args, long long), true, 10, padding, padd_width);
                        } else if (*f == 'u') {
                                buf = output64(buf, ebuf, va_arg(args, long long), false, 10, padding, padd_width);
                        } else if (*f == 'x') {
                                buf = output64(buf, ebuf, va_arg(args, long long), false, 16, padding, padd_width);
                        } else {
                                __builtin_unreachable();
                        }
                } else {
                        __builtin_unreachable();
                }
        }
        *buf = '\0';
        return (ebuf - buf);
}

int snprintf(char* buf, size_t n, const char* format, ...)
{
        va_list args;
        va_start(args, format);
        return vsnprintf(buf, n, format, args);
}
#endif
