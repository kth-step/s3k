MEMORY {
	RAM (rwx) : ORIGIN = 0x80010000, LENGTH = 0x10000
}

PROVIDE(uart_tx_one_char = 0x40000068);

__stack_size = 1024;
