#include "altc/altio.h"
#include "s3k/s3k.h"

#include "../../tutorial-commons/utils.h"

char trap_stack[1024];

int main(void)
{
	// Setup UART access
	setup_uart();
	// Setup trap handler
	setup_trap(default_trap_handler, trap_stack, 1024);


	alt_printf("Hello, world\n");
	debug_capability_from_idx(RAM_MEM);

	uint32_t free_cap = find_free_cap();
	alt_printf("Using free cap %d\n", free_cap);

	log_sys("cap derivation result", s3k_cap_derive(RAM_MEM, free_cap, 
		s3k_mk_memory(APP_1_BASE_ADDR, APP_1_BASE_ADDR + APP_1_SIZE, S3K_MEM_RWX)));
	log_sys("cap derivation result", s3k_cap_derive(RAM_MEM, find_free_cap(), 
		s3k_mk_memory(APP_1_BASE_ADDR, APP_1_BASE_ADDR + APP_1_SIZE, S3K_MEM_RWX)));

	debug_capability_from_idx(RAM_MEM);
	debug_capability_from_idx(free_cap);

	s3k_cap_t cap;

	uint32_t free_cap_pmp = find_free_cap();
	uint64_t pmp_addr = s3k_napot_encode(APP_1_BASE_ADDR, APP_1_SIZE);
	log_sys("cap derivation result",
			   s3k_cap_derive(free_cap, free_cap_pmp, s3k_mk_pmp(pmp_addr, S3K_MEM_RW)));

	debug_capability_from_idx(free_cap);
	debug_capability_from_idx(free_cap_pmp);

	log_sys("Loading the PMP", s3k_pmp_load(free_cap_pmp, 2));

	alt_printf("Sync status\n");
	s3k_sync_mem();

	alt_printf("Accessing the new memory\n");
	*((uint64_t *)(APP_1_BASE_ADDR)) = 10;
	alt_printf("Successfully wrote in random memory 0x%X\n", *((uint64_t *)(APP_1_BASE_ADDR)));
}
