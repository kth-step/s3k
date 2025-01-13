#include "drivers/uart.h"
#include "drivers/time.h"

#define UART_BASE_ADDR  0x60000000  // Example base address for UART0 (check TRM)
#define UART_FIFO_REG   (UART_BASE_ADDR + 0x0)  // TX/RX FIFO register
#define UART_CONF0_REG  (UART_BASE_ADDR + 0x20) // UART configuration
#define UART_CLKDIV_REG (UART_BASE_ADDR + 0x24) // UART clock divider register

#define UART_BAUDRATE   115200
#define APB_CLK_FREQ    80000000  // ESP32-C3 APB clock frequency

// Write a 32-bit value to a memory-mapped register
static inline void write_reg(uint32_t addr, uint32_t value) {
    *((volatile uint32_t *)addr) = value;
}

// Read a 32-bit value from a memory-mapped register
static inline uint32_t read_reg(uint32_t addr) {
    return *((volatile uint32_t *)addr);
}
uint32_t uart_tx_one_char(uint8_t TxChar);

void uart_init(void *base)
{
    uint32_t clk_div = APB_CLK_FREQ / UART_BAUDRATE;
    write_reg(UART_CLKDIV_REG, clk_div);

    // Configure data bits, parity, stop bits
    // Example: 8 data bits, no parity, 1 stop bit
    uint32_t conf0 = 0;
    conf0 |= (0 << 0);  // 8 data bits
    conf0 |= (0 << 2);  // No parity
    conf0 |= (1 << 4);  // 1 stop bit
    write_reg(UART_CONF0_REG, conf0);
}

int uart_putc(void *base, char c)
{
    // Wait until there is space in the FIFO
    while ((read_reg(UART_BASE_ADDR + 0x18) & 0xFF) >= 128);  // Check TX FIFO count

    // Write character to TX FIFO
    write_reg(UART_FIFO_REG, c);
    /* return uart_tx_one_char(c); */
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
