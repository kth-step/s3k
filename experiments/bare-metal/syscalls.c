#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

extern int uart_tx_one_char(uint8_t c);

size_t _write(int fildes __attribute__((unused)), const void *buf, size_t nbyte)
{
    const uint8_t *cbuf = (const uint8_t *) buf;
    for (size_t i = 0; i < nbyte; ++i) {
        uart_tx_one_char(cbuf[i]);
    }
    return nbyte;
}

void _exit(int exit_code __attribute__((unused)))
{
    while (true) {
        ;
    }
}

void syscalls_init(void)
{
}
