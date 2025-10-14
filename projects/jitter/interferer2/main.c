#include "s3k.h"

#include <stdio.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

volatile uint64_t buffer[96 * 1024 / sizeof(uint64_t)]; // 96KB of memory

uint64_t rdcycle(void)
{
	s3k_word_t cycle;
	__asm__ volatile("rdcycle %0" : "=r"(cycle));
	return cycle;
}

int rand(void)
{
	static uint32_t prng_state = 123456789; // Seed value
	prng_state = prng_state * 1103515245 + 12345;
	return (prng_state >> 16) & 0x7FFF;
}

// Function to flush the buffers from the cache by accessing all elements
void interfer_buffer(void)
{
	for (int i = 0; i < ARRAY_SIZE(buffer); i++) {
		// Access each buffer multiple times to ensure cache lines are evicted
		buffer[rand() % ARRAY_SIZE(buffer)]++;
	}
}

void interfer(void)
{
	interfer_buffer();
	for (int i = 0; i < rand(); i++) {
		switch (rand() % 8) {
		case 0:
			s3k_get_pid();
			break;
		case 1:
			s3k_sync();
			break;
		case 2:
			s3k_tsl_revoke(rand() % 32);
			break;
		case 3:
			s3k_mon_revoke(rand() % 32);
			break;
		case 4:
			s3k_mem_revoke(rand() % 32);
			break;
		case 5:
			s3k_ipc_revoke(rand() % 32);
			break;
		case 6:
			s3k_tsl_derive(rand() % 32, rand() % 8, rand(), rand() % 8);
			break;
		case 7:
			s3k_mem_derive(rand() % 32, rand() % 8, rand(), rand() << 12, rand() << 12);
			break;
		}
	}
}

void main(void)
{
	while (1) {
		interfer();
		s3k_sleep_until(0); // Sleep until the next cycle.
		s3k_sleep_until(0); // Sleep until the next cycle.
	}
}
