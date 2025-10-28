#include "s3k.h"

#include <stdio.h>

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

// Main server loop for IPC (Inter-Process Communication)
// Receives a server endpoint as argument
int main(s3k_word_t server)
{
	s3k_capty_t capty = 0;	// Capability type (unused in this example)
	s3k_index_t j = 0;	// Index for capability (unused in this example)
	s3k_word_t msg[2] = {}; // Message buffer for IPC
	uint32_t servtime = 32; // Minimum service time in microseconds

	while (1) {
		msg[1] = rdcycle();				      // Record cycle count before receiving
		s3k_ipc_replyrecv(server, msg, &capty, &j, servtime); // Wait for IPC call, then reply and receive next
		msg[0] = rdcycle();				      // Record cycle count after reply
		// The message buffer can be used to communicate timing information back to the
		// caller, if needed.
	}
}
