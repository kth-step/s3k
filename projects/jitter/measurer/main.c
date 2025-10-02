#include "s3k.h"
#include <stdio.h>
#include <stdlib.h>

static inline uint64_t rdcycle(void)
{
	s3k_word_t cycle;
	__asm__ volatile("rdcycle %0" : "=r"(cycle));
	return cycle;
}

static inline uint64_t diff(uint64_t a, uint64_t b)
{
	return a > b ? a - b : b - a;
}

int main(void)
{
	uint64_t expected = 32 * 1024 * 50; // 32 ms
	uint64_t last = 0;

	printf("deviation\n");
	for (int i = 0; i <= 1000; ++i) {
		s3k_sleep_until(0); // Sleep until the next cycle.
		uint64_t current = rdcycle();
		uint64_t deviation = diff(current - last, expected);
		last = current;

		// To exclude the initial measurement
		if (deviation < expected) {
			printf("%ld\n", deviation);
		}
	}
}
