#include "altc/altio.h"
#include "drivers/uart.h"
#include "plat/config.h"

void alt_init(void)
{
	uart_init((void *)UART0_BASE_ADDR);
}
