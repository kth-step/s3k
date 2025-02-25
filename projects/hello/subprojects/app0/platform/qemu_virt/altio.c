#include "altc/altio.h"

#include "altc/uart/ns16550a.h"

const struct uart_ns16550a uart = UART_NS16550A(0x10000000);

ALTFILE *const altout = (ALTFILE *const)&uart;
ALTFILE *const altio = (ALTFILE *const)&uart;
