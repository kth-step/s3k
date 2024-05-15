#include "altc/altio.h"
#include "drivers/uart.h"
#include "plat/config.h"

void alt_init(uint64_t freq, uint64_t baud)
{
	uart_init((void *)UART0_BASE_ADDR, freq, baud);
}
