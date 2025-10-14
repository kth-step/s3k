#include "s3k.h"

#include <stdio.h>

extern char __uart_base[]; // UART base address

void mem_init(s3k_word_t mon_idx, s3k_word_t idx, s3k_word_t slot, s3k_word_t cfree, s3k_word_t perm, s3k_word_t base,
	      s3k_word_t size)
{
	idx = s3k_mon_mem_derive(mon_idx, idx, cfree, perm, base, size);
	if (idx < 0) {
		printf("Failed to derive memory capability %lx\n", base);
		return;
	}

	s3k_word_t addr = s3k_pmp_napot_encode(base, size);
	int err = s3k_mon_mem_pmp_set(mon_idx, idx, slot, perm, addr);
	if (err < 0) {
		printf("Failed to set PMP for derived memory %lx, err=%d\n", base, err);
		return;
	}
}

void app2_init(void)
{
	int mon_idx = 8;   // Monitor index
	int ram_idx = 0;   // RAM index
	int uart_idx = 16; // UART index

	// RAM configuration
	s3k_word_t ram_base = 0x80020000;
	s3k_word_t ram_size = 0x10000;
	s3k_word_t ram_perm = S3K_MEM_PERM_RWX; // Read/Write/Execute permissions
	s3k_word_t ram_fuel = 1;
	s3k_word_t ram_slot = 1;
	mem_init(mon_idx, ram_idx, ram_slot, ram_fuel, ram_perm, ram_base, ram_size);

	s3k_word_t uart_base = (s3k_word_t)__uart_base;
	s3k_word_t uart_size = 0x20;
	s3k_word_t uart_perm = S3K_MEM_PERM_RW; // Read/Write permissions
	s3k_word_t uart_fuel = 1;
	s3k_word_t uart_slot = 2;
	mem_init(mon_idx, uart_idx, uart_slot, uart_fuel, uart_perm, uart_base, uart_size);

	if (s3k_mon_reg_set(mon_idx, S3K_REG_PC, ram_base) != 0) {
		printf("Failed to set program counter to RAM start\n");
		return;
	}
}
