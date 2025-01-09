#include "drivers/uart.h"
#include "drivers/time.h"

uint32_t uart_tx_one_char(uint8_t TxChar);

void uart_init(void *base)
{

}

int uart_putc(void *base, char c)
{
   return uart_tx_one_char(c);
}

s3k_time_t time_get(void)
{
	return 0;
}

void time_set(s3k_time_t time)
{
}

s3k_time_t timeout_get(s3k_hart_t hartid)
{
  return 0;
}

void timeout_set(s3k_hart_t hartid, s3k_time_t timeout)
{
}
