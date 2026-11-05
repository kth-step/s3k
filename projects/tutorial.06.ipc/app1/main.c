#include "s3k.h"

#include <stdio.h>

extern void app2_init(void); // Function to initialize app2 (the server application)

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

#define PROC_STATE_BLOCKED 2

int main(void)
{
	setup_uart(MAX_MEMORY_FUEL);
	s3k_sync();
	printf("IPC Application \n");
	app2_init(); // Initialize the server application

	// Set up IPC endpoints and grant permissions
	s3k_ipc_flag_t flags = S3K_IPC_FLAG_YIELD;
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

	printf("call,replyrecv\n");
	for (int i = 0; i < 100; ++i) {
		s3k_sleep_until(0); // Synchronize to the next time slot

		s3k_msg_t msg = {};
		msg.capty = 0;
		msg.capidx = 0;
		msg.data[0] = i;
		s3k_ipc_call(client, &msg);
		printf("received %d\n", msg.data[0]); // Output results
	}

	// Suspend server and client
	s3k_mon_suspend(8);
	s3k_mon_suspend(0);
	s3k_sync();
}
