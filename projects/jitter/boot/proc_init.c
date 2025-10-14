#include "s3k.h"

#include <stdio.h>

extern char __uart_base[]; // UART base address

static bool mem_init(s3k_word_t mon_idx, s3k_word_t idx, s3k_word_t slot, s3k_word_t cfree, s3k_word_t perm,
		     s3k_word_t base, s3k_word_t size)
{
	// Derive a memory capability
	idx = s3k_mon_mem_derive(mon_idx, idx, cfree, perm, base, size);
	if (idx < 0) {
		printf("Failed to derive memory capability %lx\n", base);
		return false;
	}

	// Set PMP (Physical Memory Protection) for the derived memory
	s3k_word_t addr = s3k_pmp_napot_encode(base, size);
	int err = s3k_mon_mem_pmp_set(mon_idx, idx, slot, perm, addr);
	if (err < 0) {
		printf("Failed to set PMP for derived memory %lx, err=%d\n", base, err);
		return false;
	}

	return true;
}

/**
 * Initialize the process controlled by mon_idx with memory regions for RAM and UART.
 */
void proc_init(s3k_word_t mon_idx, int ram_idx, s3k_word_t ram_base, s3k_word_t ram_size)
{
	// Indices for memory regions
	int uart_idx = 16; // UART index

	// RAM configuration parameters
	s3k_word_t ram_perm = S3K_MEM_PERM_RWX; // Read/Write/Execute permissions
	s3k_word_t ram_fuel = 1;
	s3k_word_t ram_slot = 1;

	// Initialize RAM memory
	if (!mem_init(mon_idx, ram_idx, ram_slot, ram_fuel, ram_perm, ram_base, ram_size)) {
		printf("Failed to initialize RAM memory\n");
		return;
	}

	// UART configuration parameters
	s3k_word_t uart_base = (s3k_word_t)__uart_base;
	s3k_word_t uart_size = 0x20;
	s3k_word_t uart_perm = S3K_MEM_PERM_RW; // Read/Write permissions
	s3k_word_t uart_fuel = 1;
	s3k_word_t uart_slot = 2;

	// Initialize UART memory
	if (!mem_init(mon_idx, uart_idx, uart_slot, uart_fuel, uart_perm, uart_base, uart_size)) {
		printf("Failed to initialize UART memory\n");
		return;
	}

	// Set the program counter to the start of RAM
	if (s3k_mon_reg_set(mon_idx, S3K_REG_PC, ram_base) != 0) {
		printf("Failed to set program counter to RAM start\n");
		return;
	}
}
