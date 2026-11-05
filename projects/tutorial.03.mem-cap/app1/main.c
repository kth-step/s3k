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

static s3k_word_t trap_stack[1024] __attribute__((aligned(16)));

static void trap_handler(void) __attribute__((interrupt("machine")));

static void trap_handler(void)
{
	s3k_word_t epc = s3k_vreg_get(S3K_VREG_EPC);
	s3k_word_t esp = s3k_vreg_get(S3K_VREG_ESP);
	s3k_word_t ecause = s3k_vreg_get(S3K_VREG_ECAUSE);
	s3k_word_t eval = s3k_vreg_get(S3K_VREG_EVAL);

	printf("error info:\n- epc: 0x%lx\n- esp: 0x%lx\n- ecause: 0x%lx\n- eval: 0x%lx\n", epc, esp, ecause, eval);
	printf("restoring pc and sp\n\n");
}

void setup_trap(void (*trap_handler)(void), void *trap_stack_base, uint64_t trap_stack_size)
{
	// Sets the trap handler
	s3k_vreg_set(S3K_VREG_TPC, (uint64_t)trap_handler);
	// Set the trap stack
	s3k_vreg_set(S3K_VREG_TSP, ((uint64_t)trap_stack_base) + trap_stack_size);
}

static void dump_memory_cap(int idx)
{
	s3k_cap_mem_t cap = {};
	s3k_pmp_slot_t slot = 0;
	s3k_mem_perm_t perm = 0;
	s3k_pmp_addr_t addr = 0;

	int rc = s3k_mem_get(idx, &cap);
	printf("mem[%d] get rc=%d owner=%u cfree=%u csize=%u slot=%u perm=0x%x begin=0x%lx end=0x%lx\n", idx, rc,
	       (unsigned)cap.owner, (unsigned)cap.cfree, (unsigned)cap.csize, (unsigned)cap.slot, (unsigned)cap.rwx,
	       (unsigned long)cap.begin, (unsigned long)cap.end);

	rc = s3k_mem_pmp_get(idx, &slot, &perm, &addr);
	printf("mem[%d] pmp rc=%d slot=%u perm=0x%x addr=0x%lx base=0x%lx size=0x%lx\n", idx, rc, (unsigned)slot,
	       (unsigned)perm, (unsigned long)addr, (unsigned long)s3k_pmp_napot_decode_base(addr),
	       (unsigned long)s3k_pmp_napot_decode_size(addr));
}

int main(void)
{
	setup_trap(trap_handler, trap_stack, 1024);
	setup_uart(MAX_MEMORY_FUEL);
	printf("Hello, world\n");

	dump_memory_cap(0);
	dump_memory_cap(MAX_MEMORY_FUEL);

	// RAM configuration
	s3k_word_t ram_base = 0x80000000 + 0x1000000;
	s3k_word_t ram_size = 0x10000;
	s3k_word_t ram_perm = S3K_MEM_PERM_RWX; // Read/Write/Execute permissions
	s3k_word_t ram_fuel = 2;		// size
	s3k_word_t ram_slot = 3;		// pmp slot
	s3k_word_t idx = s3k_mem_derive(0, ram_fuel, ram_perm, ram_base, ram_size);
	if (idx < 0) {
		printf("Failed to derive memory capability %lx\n", ram_base);
		return 1;
	}
	printf("New capability index %ld\n", idx);
	dump_memory_cap(idx);

	s3k_word_t napot_addr = s3k_pmp_napot_encode(ram_base, ram_size);
	int err = s3k_mem_pmp_set(idx, ram_slot, ram_perm, napot_addr);
	if (err < 0) {
		printf("Failed to set PMP for derived memory %lx, err=%d\n", ram_base, err);
		return 2;
	}
	dump_memory_cap(idx);
	s3k_sync();
	*((uint64_t *)(ram_base)) = 0x10;
	printf("Successfully wrote in random memory 0x%lx\n", *((uint64_t *)(ram_base)));

	printf("memory capability demo complete");
}
