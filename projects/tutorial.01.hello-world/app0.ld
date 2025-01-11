MEMORY {
	IRAM (rwx) : ORIGIN = 0x42010000, LENGTH = 0x10000
    RAM (rw): org = 0x3FC80000 + 0x10000, len = 0x10000
}

PROVIDE(uart_tx_one_char = 0x40000068);

__stack_size = 1024;
