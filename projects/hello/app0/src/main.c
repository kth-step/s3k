#include "altio/altio.h"
#include "s3k/s3k.h"

int main(void)
{
	uint64_t uart_addr = s3k_napot_encode(0x10000000, 0x8);
	while (s3k_cap_derive(2, 16, s3k_pmp_cap(uart_addr, S3K_MEM_RW)))
		;
	while (s3k_cap_derive(2, 18, s3k_pmp_cap(uart_addr, S3K_MEM_RW)))
		;
	while (s3k_pmp_load(16, 1))
		;
	s3k_sync();
	alt_puts("hello, world from app0");

	uint64_t app1_addr = s3k_napot_encode(0x80020000, 0x10000);
	s3k_error_t err;
	while (s3k_cap_derive(1, 17, s3k_pmp_cap(app1_addr, S3K_MEM_RWX)))
		alt_puts("err0");
	while (s3k_mon_cap_move(8, 0, 17, 1, 0))
		alt_puts("err1");
	while (s3k_mon_cap_move(8, 0, 18, 1, 1))
		alt_puts("err2");
	while (s3k_cap_derive(4, 19, s3k_time_cap(0, 0, 15)))
		alt_puts("err2a");
	while (s3k_mon_cap_move(8, 0, 19, 1, 2))
		alt_puts("err2b");
	while (s3k_mon_pmp_load(8, 1, 0, 0))
		alt_puts("err3");
	while (s3k_mon_pmp_load(8, 1, 1, 1))
		alt_puts("err4");
	while ((err = s3k_mon_reg_write(8, 1, 0, 0x80020000)))
		if (err != ERR_PREEMPTED)
			alt_puts("err4b");
	while ((err = s3k_mon_resume(8, 1)))
		if (err != ERR_PREEMPTED)
			alt_puts("err5");
}
