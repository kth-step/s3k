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

// Issue a temporal fence (barrier) instruction
void temporal_fence(void)
{
	__asm__ volatile(".word 0xb");
}

// Run a single IPC test between client and server, measuring timing
void run_test(int client, uint64_t res[3], s3k_capty_t capty, int j)
{
	uint64_t msg[2];

#ifdef COLD
	// If COLD is defined, insert a temporal fence before measurement
	temporal_fence();
#else
	// Otherwise, perform 5 warm-up IPC calls before measurement
	for (int i = 0; i < 5; ++i)
		s3k_ipc_call(client, msg, &capty, &j);
#endif

	// Measure the cost of a single IPC call and replyrecv
	uint64_t start = rdcycle();
	s3k_ipc_call(client, msg, &capty, &j);
	uint64_t end = rdcycle();
	res[0] = msg[0] - start;  // Time from start to server reply
	res[1] = end - msg[1];	  // Time from server reply to end
	res[2] = res[0] + res[1]; // Total round-trip time
}

int main(void)
{
	app2_init(); // Initialize the server application

	// Set up IPC endpoints and grant permissions
	s3k_ipc_flag_t flags = S3K_IPC_FLAG_YIELD | S3K_IPC_FLAG_TSL;
	s3k_ipc_mode_t mode = S3K_IPC_MODE_BSYNC;
	int server = s3k_ipc_derive(0, 2, mode, flags);	     // Derive server endpoint
	int client = s3k_ipc_derive(server, 1, mode, flags); // Derive client endpoint

	s3k_mon_ipc_grant(8, server);		// Grant server endpoint to monitor 8
	s3k_mon_reg_set(8, S3K_REG_A0, server); // Set server endpoint in monitor 8's register
	s3k_mon_resume(8);			// Resume monitor 8 (server)
	s3k_mon_yield(8);			// Yield to monitor 8
	s3k_mon_yield(8);			// Yield to monitor 8
	s3k_mon_yield(8);			// Yield to monitor 8

#ifdef SEND_CAP
	// If SEND_CAP is defined, derive a time-slice capability and set type
	s3k_index_t j = s3k_tsl_derive(0, 1, true, 4);
	s3k_capty_t capty = S3K_CAPTY_TSL;
#else
	// Otherwise, use no capability
	s3k_capty_t capty = 0;
	s3k_index_t j;
#endif

	uint64_t res[3];
	printf("call,replyrecv,rtt\n");
	for (int i = 0; i < 100; ++i) {
		s3k_sleep_until(0);				 // Synchronize to the next time slot
		run_test(client, res, capty, j);		 // Run IPC test and collect timing
		printf("%ld,%ld,%ld\n", res[0], res[1], res[2]); // Output results as comma-separated values
	}

	// Suspend server and client
	s3k_mon_suspend(8);
	s3k_mon_suspend(0);
	s3k_sync();
}
