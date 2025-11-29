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
	s3k_msg_t msg = {};
	msg.servtime = 32;

	while (1) {
		// Record cycle count before receiving
		msg.data[1] = rdcycle();
		s3k_ipc_replyrecv(server, &msg);
		msg.data[0] = rdcycle();
		// Record cycle count after reply
		// The message buffer can be used to communicate timing information back to the
		// caller, if needed.
	}
}
