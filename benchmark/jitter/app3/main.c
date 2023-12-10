#include "altc/altio.h"
#include "s3k/s3k.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define L1_TOTAL_SIZE (32 * 1024)
#define L1_LINE_SIZE 64

static volatile uint64_t data[L1_TOTAL_SIZE / sizeof(uint64_t)];

uint64_t random(void)
{
	static unsigned long x = 123456789, y = 362436069, z = 521288629;
	unsigned long t;
	x ^= x << 16;
	x ^= x >> 5;
	x ^= x << 1;

	t = x;
	x = y;
	y = z;
	z = t ^ x ^ y;

	return z;
}

void flush_data_cache()
{
	// 32 KiB 8-way cache.
	for (int i = 0; i < L1_TOTAL_SIZE; i += L1_LINE_SIZE) {
		data[i / sizeof(uint64_t)] = random();
	}
}

int main(void)
{
	while (1)
		flush_data_cache();
}
