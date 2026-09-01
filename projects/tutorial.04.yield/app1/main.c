#include "s3k.h"

#include <stdio.h>


extern char __uart_base[]; // UART base address.

static void setup_uart(int idx)
{
	s3k_word_t base = (s3k_word_t)__uart_base;
	s3k_word_t size = 0x20;
	s3k_word_t slot = 2;
	s3k_word_t perm = S3K_MEM_PERM_RW; // Read/write permissions.
	s3k_word_t addr = s3k_pmp_napot_encode(base, size);

	s3k_mem_pmp_set(idx, slot, perm, addr);
	s3k_sync();
	puts("uart set up!");
}

static void dump_timing_cap(int idx)
{
	s3k_cap_tsl_t cap = {};
	int rc = s3k_tsl_get(idx, &cap);

	printf("tsl[%d] get rc=%d owner=%u cfree=%u csize=%u hart=%u enabled=%u mark=%u begin=%u end=%u\n",
	       idx, rc, (unsigned)cap.owner, (unsigned)cap.cfree, (unsigned)cap.csize, (unsigned)cap.hart,
	       (unsigned)cap.enabled, (unsigned)cap.mark, (unsigned)cap.begin, (unsigned)cap.end);
}


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
	int mon_idx = S3K_BOOT_MON_P2_IDX;   // Monitor index
	int ram_idx = S3K_BOOT_MEM_RAM_IDX;   // RAM index
	int uart_idx = S3K_BOOT_MEM_UART_IDX; // UART index

	// RAM configuration
	s3k_word_t ram_base = APP2_RAM_ORIGIN;
	s3k_word_t ram_size = APP2_RAM_LENGTH;
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
	if 	(s3k_mon_resume(mon_idx) != 0) {
		printf("Failed to resume\n");
		return;
	}
}

int main(void)
{
	setup_uart(S3K_BOOT_MEM_UART_IDX);

	puts("Monitor demo");
	dump_timing_cap(0);

	app2_init();
	puts("Second process ready");
	s3k_sync();
	while (true) {
		puts("yield to second process\n");
		int err;
		err = s3k_mon_yield(S3K_BOOT_MON_P2_IDX);
		if (err < 0) {
			printf("Failed to yield, err=%d\n", err);
			return 0;
		}
	}
}
