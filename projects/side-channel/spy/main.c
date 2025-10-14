#include "s3k.h"

#include <stdio.h>
#include <stdlib.h>

// Buffer used for cache measurement. Size: 1 KiB (divided by uint64_t size).
volatile uint64_t buffer[1 * 1024 / sizeof(uint64_t)];

// Read the current cycle counter using the RISC-V rdcycle instruction.
static inline uint64_t rdcycle(void)
{
	s3k_word_t cycle;
	__asm__ volatile("rdcycle %0" : "=r"(cycle));
	return cycle;
}

// Measure the time (in cycles) to repeatedly access the buffer.
// This is used to estimate cache access time.
uint64_t measure_cache(void)
{
	uint64_t start = rdcycle();
	for (volatile int i = 0; i < 1000; ++i) {
		buffer[i % (sizeof(buffer) / sizeof(buffer[0]))] = i;
	}
	uint64_t end = rdcycle();
	return end - start;
}

// Measure the time (in cycles) to perform two system calls:
// s3k_tsl_derive and s3k_tsl_revoke.
uint64_t measure_syscall(void)
{
	uint64_t start = rdcycle();
	s3k_tsl_derive(8, 1, 1, 1);
	s3k_tsl_revoke(8);
	uint64_t end = rdcycle();
	return end - start;
}

int main(void)
{
	uint64_t period = 32 * 1024 * 50; // Target period: 32 ms in cycles.
	s3k_sleep_until(0);		  // Synchronize to the next minor frame.
	uint64_t base = rdcycle();	  // Record the base cycle count.

	for (int i = 0; i < 1000; ++i) {
		s3k_sleep_until(0);				 // Wait for the next minor frame.
		uint64_t current = rdcycle();			 // Read current cycle count.
		base += period;					 // Advance the expected base by one period.
		uint64_t jitter = current - base;		 // Calculate timing jitter.
		uint64_t cache = measure_cache();		 // Measure cache access time.
		uint64_t syscall = measure_syscall();		 // Measure syscall time.
		printf("%ld,%ld,%ld\n", jitter, cache, syscall); // Output results as CSV.
	}
}
