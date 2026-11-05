#include "s3k.h"

#include <stdio.h>

extern char __uart_base[]; // UART base address
			   //

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

static s3k_word_t trap_stack[1024] __attribute__((aligned(16)));

static void trap_handler(void) __attribute__((interrupt("machine")));

static void trap_handler(void)
{
	setup_uart(MAX_MEMORY_FUEL);

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

int main(void)
{
	setup_trap(trap_handler, trap_stack, 1024);
	puts("Hello world from app2!");
}
