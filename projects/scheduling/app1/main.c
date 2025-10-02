#include <stdio.h>
#include "s3k_inline.h"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

extern char __uart_base[]; // UART base address

// Max number of time slices created.
#define MAX_TSLS 16
// Max number of time slots allocatable.
#define MAX_SLOTS 64
// Number of tests
#define ROUNDS 100

volatile uint64_t rdcycle(void)
{
	register uintptr_t cycle;
	__asm__ volatile("rdcycle %0" : "=r"(cycle));
	return cycle;
}

#ifdef RAND

int rand(void) {
    static uint32_t prng_state = 123456789; // Seed value
    prng_state = prng_state * 1103515245 + 12345;
    return (prng_state >> 16) & 0x7FFF;
}

uint64_t test_round(int tsls, int ts)
{
	uint64_t start, end;
	int size[MAX_TSLS];

	// Children are not allowed to create new time slices.
	int csize = 1;

	s3k_sleep_until(0);

	for (int i = 0; i < tsls; i++) {
		size[i] = (rand() % (MAX_SLOTS / tsls)) + 1;
	}

	start = rdcycle();

	// Delete previous scheduling.
	s3k_tsl_revoke(ts);

	// Apply new schedule.
	for (int i = 1; i <= tsls; i++) {
		s3k_mon_tsl_derive(i * 8, ts, csize, true, size[i - 1]);
	}

	end = rdcycle();
	return (end - start);
}

#else

uint64_t test_round(int tsls, int ts)
{
	// size >= 4 maximizes cache usage.
	// Each scheduling frame is 4 bytes, each cache line is 16 bytes.
	int size = MAX_SLOTS / ts;

	// Children are not allowed to create new time slices.
	int csize = 1;

	s3k_sleep_until(0);

	uint64_t start = rdcycle();

	// Delete previous scheduling.
	s3k_tsl_revoke(ts);

	// Apply new schedule.
	for (int i = 1; i <= tsls; i++) {
		s3k_mon_tsl_derive(i * 8, ts, csize, true, size);
	}

	uint64_t end = rdcycle();
	return end - start;
}
#endif

void test(int rounds, int tsls, int ts)
{
	// Dry run
	s3k_sleep_until(0);

	// Do a 10 warm-up rounds first.
	uint64_t res[ROUNDS+10];
	for (int i = 0; i < ROUNDS + 10; i++) {
		res[i] = test_round(tsls, ts);
	}
	s3k_tsl_revoke(ts);

	// Overwrite warm-up rounds
	for (int i = 0; i < ROUNDS; ++i) {
		res[i] = res[i + 10];
	}

	uint64_t avg = 0;
	uint64_t max = 0;
	uint64_t min = UINT64_MAX;
	for (int i = 0; i < rounds; i++) {
		if (res[i] > max) {
			max = res[i];
		}
		if (res[i] < min) {
			min = res[i];
		}
		avg += res[i];
	}
	avg /= rounds;

	printf("%d,%ld,%ld,%ld\n", tsls, avg, max, min);
}

int main(void)
{
	printf("tsls,avg,max,min\n");

	// Derive a time slice with csize=33, and size=72
	int ts = s3k_tsl_derive(0, 33, false, 72);

	int tsls[] = {1, 2, 4, 8, 16};
	for (int i = 0; i < ARRAY_SIZE(tsls); i++) {
		test(ROUNDS, tsls[i], ts);
	}

	s3k_tsl_revoke(0);

	s3k_mon_suspend(0);
}
