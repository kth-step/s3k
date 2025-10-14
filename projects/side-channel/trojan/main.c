#include "s3k.h"

#include <stdio.h>

// Macro to compute the number of elements in an array
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

volatile uint64_t buffer0[8 * 1024 / sizeof(uint64_t)] __attribute__((aligned(32 * 1024)));
volatile uint64_t buffer1[8 * 1024 / sizeof(uint64_t)] __attribute__((aligned(32 * 1024)));
volatile uint64_t buffer2[8 * 1024 / sizeof(uint64_t)] __attribute__((aligned(32 * 1024)));
volatile uint64_t buffer3[8 * 1024 / sizeof(uint64_t)] __attribute__((aligned(32 * 1024)));

// Simple pseudo-random number generator (LCG)
int rand(void)
{
	static uint32_t prng_state = 123456789; // Seed value
	prng_state = prng_state * 1103515245 + 12345;
	return (prng_state >> 16) & 0x7FFF;
}

// Flush the cache
void flush(void)
{
	for (int i = 0; i < ARRAY_SIZE(buffer0); i++) {
		buffer0[i]++;
		buffer1[i]++;
		buffer2[i]++;
		buffer3[i]++;
	}
}

// Main function: generates a pseudo-random bitstream and triggers cache flushes
void main(void)
{
	s3k_sleep_until(0); // Synchronize to the next minor frame.
	for (int i = 0; i < 1000; ++i) {
		s3k_sleep_until(0); // Wait for the next minor frame.
		int r = rand() % 2; // Generate a random bit (0 or 1).
		printf("%d,", r);   // Output the bit as CSV, this is the signal (Y).
		if (r) {
			flush(); // If bit is 1, flush the caches, this is the covert channel.
			flush(); // If bit is 1, flush the caches, this is the covert channel.
			flush(); // If bit is 1, flush the caches, this is the covert channel.
			flush(); // If bit is 1, flush the caches, this is the covert channel.
		}
	}
}
