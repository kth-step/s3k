// See LICENSE file for copyright and license details.
#ifndef NDEBUG
#include "kprint.h"
#include "lock.h"
#include "uart.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static lock_t lock = INIT_LOCK;

int write(const char* buf, int nbytes) {
        lock_acquire(&lock);
        int i = uart_write(buf, nbytes);
        lock_release(&lock);
        return i;
}

int puts(const char* s)
{
        lock_acquire(&lock);
        int i = uart_puts(s);
        lock_release(&lock);
        return i;
}

int kprintf(const char* restrict format, ...)
{
        va_list args;
        char buf[256];
        va_start(args, format);
        int len = vsnprintf(buf, 256, format, args);
        va_end(args);
        return write(buf, len);
}

static inline void outputStr(char* buf, int *j, int nbytes, const char* s, char padding, int padd_width)
{
        int len;
        for (len = 0; s[len] != '\0'; len++);
        padd_width -= len;
        for (int i = 0; i < padd_width; i++)
                buf[(*j)++] = padding;

        while (*s != '\0' && *j < nbytes)
                buf[(*j)++] = *s++;

}

static inline void outputNum(char* buf, int *j, int nbytes, unsigned long long num, bool neg, int base, char padding, int padd_width)
{
        char nbuf[64];
        nbuf[63] = '\0';
        int i = 63;
        do {
                unsigned long long tmp = num % base;
                if (tmp >= 10)
                        nbuf[--i] = 'a' + tmp - 10;
                else
                        nbuf[--i] = '0' + tmp;
                num /= base;
        } while (num != 0);

        if (neg && *j < nbytes && padding == '0')
                buf[(*j)++] = '-';
        else if (neg && padding == ' ')
                nbuf[--i] = '-';

        outputStr(buf, j, nbytes, &nbuf[i], padding, padd_width);
}

int vsnprintf(char* buf, size_t nbytes, const char* format, va_list args)
{
        if (nbytes == 0)
                return 0;
        nbytes = nbytes - 1;
        int i = 0, f = 0;
        while (format[f] != '\0' && i < nbytes) {
                if (format[f] != '%') {
                        buf[i++] = format[f++];
                        continue;
                }
                f++;
                char padding = ' ';
                int padd_width = 0;
                if (format[f] == '0') {
                        padding = '0';
                        f++;
                }
                while (format[f] >= '0' && format[f] <= '9') {
                        padd_width *= 10;
                        padd_width += format[f] - '0';
                        f++;
                }
                switch (format[f++]) {
                case 'd': {
                                  long long num = va_arg(args, int);
                                  bool neg = num < 0;
                                  num = neg ? -num : num;
                        outputNum(buf, &i, nbytes, 0xFFFFFFFF & num, neg, 10, padding, padd_width);
                        break;
                }
                case 'u':
                        outputNum(buf, &i, nbytes, 0xFFFFFFFF & va_arg(args, int) , false, 10, padding, padd_width);
                        break;
                case 'o':
                        outputNum(buf, &i, nbytes, 0xFFFFFFFF & va_arg(args, int) , false, 8, padding, padd_width);
                        break;
                case 'x':
                        outputNum(buf, &i, nbytes, 0xFFFFFFFF & va_arg(args, int) , false, 16, padding, padd_width);
                        break;
                case 's':
                        outputStr(buf, &i, nbytes, va_arg(args, char*), ' ', padd_width);
                        break;
                case 'c':
                        buf[i] = va_arg(args, int);
                        break;
                case '%':
                        buf[i] = '%';
                        break;
                case 'l':
                        switch (format[f++]) {
                                case 'd': {
                                                  long long num = va_arg(args, long);
                                                  bool neg = num < 0;
                                                  num = neg ? -num : num;
                                                  outputNum(buf, &i, nbytes, num, neg, 10, padding, padd_width);
                                                  break;
                                          }
                                case 'u':
                                          outputNum(buf, &i, nbytes, va_arg(args, unsigned long) , false, 10, padding, padd_width);
                                          break;
                                case 'o':
                                          outputNum(buf, &i, nbytes, va_arg(args, unsigned long) , false, 8, padding, padd_width);
                                          break;
                                case 'x':
                                          outputNum(buf, &i, nbytes, va_arg(args, unsigned long) , false, 16, padding, padd_width);
                                          break;
                                default:
                                          __builtin_unreachable();
                        }
                        break;
                default:
                        __builtin_unreachable();
                }
        }
        return i;
}

int snprintf(char* buf, size_t n, const char* format, ...)
{
        va_list args;
        va_start(args, format);
        int len = vsnprintf(buf, n, format, args);
        va_end(args);
        return len;
}
#endif
