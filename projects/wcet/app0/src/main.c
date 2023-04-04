#include "altio.h"
#include "s3k.h"
#include "timer.h"

#include <stdint.h>

int main(void)
{
	// Access UART and timer
	uint64_t uart_addr = s3k_napot_encode(0x10000000, 0x8);
	uint64_t time_addr = s3k_napot_encode(0x200bff8, 0x8);
	s3k_drvcap(2, 16, s3k_mkpmp(uart_addr, S3K_RW));
	s3k_drvcap(3, 17, s3k_mkpmp(time_addr, 1));
	s3k_pmpset(16, 1);
	s3k_pmpset(17, 2);

	alt_puts("hello, world");
	uint64_t old = -1;
	uint64_t start, end, elapsed, wcet;
	s3k_cap_t time = s3k_mktime(1, 0, 64);
	while (1) {
		s3k_revcap(5);
		while (s3k_drvcap(5, 18, time))
			;
		asm volatile("csrr %0,cycle" : "=r"(start));
		s3k_revcap(5);
		asm volatile("csrr %0,cycle" : "=r"(end));
		elapsed = end - start;
		if (old == elapsed)
			continue;
		asm volatile("csrr %0,hpmcounter3" : "=r"(wcet));
		alt_printf("%X   %X\n", elapsed, wcet);
		old = elapsed;
	}
}
