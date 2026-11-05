#include "s3k.h"

#include <stdio.h>

extern void app2_init(void); // Function to initialize app2 (the server application)

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

extern char __uart_base[]; // UART base address, provided by the linker

// Read the current cycle counter using the RISC-V rdcycle instruction
uint64_t rdcycle(void)
{
	s3k_word_t cycle;
	__asm__ volatile("rdcycle %0" : "=r"(cycle));
	return cycle;
}

#define CAPTY_MEM 0x1

// Run a single IPC test between client and server, measuring timing
void run_test(int client, int buffer_cap, uint64_t res[3])
{
	s3k_msg_t msg = {};

	printf("Mem revoke %lx\n", s3k_mem_revoke(buffer_cap));

	// Measure the cost of a single IPC call and replyrecv
	// RAM configuration
	s3k_word_t ram_base = 0x80000000 + 0x1000000;
	s3k_word_t ram_size = 0x10000;
	s3k_word_t ram_perm = S3K_MEM_PERM_RWX; // Read/Write/Execute permissions
	s3k_word_t ram_fuel = 1;		// size
	s3k_word_t ram_slot = 3;		// pmp slot
	s3k_word_t idx = s3k_mem_derive(buffer_cap, ram_fuel, ram_perm, ram_base, ram_size);
	if (idx < 0) {
		printf("Failed to derive memory capability %lx\n", ram_base);
		return;
	}
	printf("New capability index %ld\n", idx);

	s3k_word_t napot_addr = s3k_pmp_napot_encode(ram_base, ram_size);
	int err = s3k_mem_pmp_set(idx, ram_slot, ram_perm, napot_addr);
	if (err < 0) {
		printf("Failed to set PMP for derived memory %lx, err=%d\n", ram_base, err);
		return;
	}
	s3k_sync();
	*((uint64_t *)(ram_base)) = *((uint64_t *)(ram_base)) + 1;
	printf("Successfully wrote in random memory 0x%lx\n", *((uint64_t *)(ram_base)));

	msg.capty = CAPTY_MEM;
	msg.capidx = idx;

	uint64_t start = rdcycle();
	s3k_ipc_call(client, &msg);
	uint64_t end = rdcycle();
	res[0] = end - start; // Round trip time
	res[1] = msg.data[0];
	res[2] = msg.data[1];
}

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

#define PROC_STATE_BLOCKED 2

int main(void)
{
	setup_uart(MAX_MEMORY_FUEL);
	s3k_sync();
	printf("IPC Application \n");
	app2_init(); // Initialize the server application

	// Set up IPC endpoints and grant permissions
	s3k_ipc_flag_t flags = S3K_IPC_FLAG_YIELD | S3K_IPC_FLAG_MEM;
	s3k_ipc_mode_t mode = S3K_IPC_MODE_BSYNC;
	int server = s3k_ipc_derive(0, 2, mode, flags);	     // Derive server endpoint
	int client = s3k_ipc_derive(server, 1, mode, flags); // Derive client endpoint

	s3k_mon_ipc_grant(8, server);		// Grant server endpoint to monitor 8
	s3k_mon_reg_set(8, S3K_REG_A0, server); // Set server endpoint in monitor 8's register
	s3k_mon_resume(8);			// Resume monitor 8 (server)

	while (true) {
		s3k_mon_yield(8); // Yield to monitor 8
		uint64_t state;
		s3k_mon_state_get(8, &state);
		if (state & PROC_STATE_BLOCKED)
			break;
	}
	printf("Process 2 is ready to receive\n");

	s3k_capty_t capty = 0;
	s3k_index_t j = 0;

	s3k_word_t ram_base = 0x80000000 + 0x1000000;
	s3k_word_t ram_size = 0x10000;
	s3k_word_t ram_perm = S3K_MEM_PERM_RWX; // Read/Write/Execute permissions
	s3k_word_t ram_fuel = 2;		// size
	s3k_word_t buffer_cap = s3k_mem_derive(0, ram_fuel, ram_perm, ram_base, ram_size);

	uint64_t res[3];
	printf("call,replyrecv,rtt\n");
	for (int i = 0; i < 100; ++i) {
		s3k_sleep_until(0);				 // Synchronize to the next time slot
		run_test(client, buffer_cap, res);		 // Run IPC test and collect timing
		printf("%ld,%ld,%ld\n", res[0], res[1], res[2]); // Output results as comma-separated values
	}

	// Suspend server and client
	s3k_mon_suspend(8);
	s3k_mon_suspend(0);
	s3k_sync();
}
