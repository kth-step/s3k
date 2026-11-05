#include "s3k.h"

#include <stdio.h>

extern char __uart_base[]; // UART base address

// Enable memory at mem capability idx
void setup_uart(int idx)
{
	s3k_word_t base = (s3k_word_t)__uart_base;
	s3k_word_t size = 0x20;
	s3k_word_t slot = 2;
	s3k_word_t perm = S3K_MEM_PERM_RW; // Read/Write permissions
	s3k_word_t addr = s3k_pmp_napot_encode(base, size);
	s3k_mem_pmp_set(idx, slot, perm, addr);
	s3k_sync();
	puts("uart set up!");
	return;
}

int main(void)
{
	setup_uart(MAX_MEMORY_FUEL);
	printf("hello, world\n");
}
