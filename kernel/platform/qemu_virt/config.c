#include "drivers/uart.h"

const struct uart uart = UART_NS16550A(0x10000000);

struct uart *const kout = (struct uart *const)&uart;
