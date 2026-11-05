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
	// Minimal service time
	msg.servtime = 32;

	while (1) {
		// Record cycle count before receiving
		s3k_ipc_replyrecv(server, &msg);

		s3k_word_t ram_base = 0x80000000 + 0x1000000;
		s3k_word_t ram_size = 0x10000;
		s3k_word_t ram_perm = S3K_MEM_PERM_RWX; // Read/Write/Execute permissions
		printf("App2: Received cap %d\n", msg.capidx);
		s3k_word_t napot_addr = s3k_pmp_napot_encode(ram_base, ram_size);
		printf("app2 after encode\n");
		int err = s3k_mem_pmp_set(msg.capidx, 3, ram_perm, napot_addr);
		printf("app2 after pmp set\n");
		if (err < 0) {
			printf("Failed to set PMP for derived memory %lx, err=%d\n", ram_base, err);
			return 2;
		}
		/* s3k_mem_sync(); */
		printf("app2 after sync\n");
		msg.capty = 0;
		msg.data[0] = *((uint64_t *)(ram_base));
		printf("Successfully read in random memory 0x%lx\n", *((uint64_t *)(ram_base)));
		// Record cycle count after reply
		// The message buffer can be used to communicate timing information back to the
		// caller, if needed.
	}
}
