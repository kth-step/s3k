// See LICENSE file for copyright and license details.
#include "kprint.h"

#ifndef NDEBUG
#include "lock.h"
#include "snprintf.h"
#include "uart.h"

int puts(const char* s)
{
        static lock_t lock = INIT_LOCK;
        lock_acquire(&lock);
        int i = uart_puts(s);
        lock_release(&lock);
        return i;
}

int kprintf(const char* format, ...)
{
        va_list args;
        char buf[256];
        va_start(args, format);
        vsnprintf(buf, 256, format, args);
        va_end(args);
        return puts(buf);
}
#endif
