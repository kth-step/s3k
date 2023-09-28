#include "altio/altio.h"
#include "s3k/s3k.h"

int main(void)
{
	uint64_t uart_addr = s3k_napot_encode(0x10000000, 0x8);
	uint64_t app1_addr = s3k_napot_encode(0x80020000, 0x10000);

	s3k_cap_derive(2, 16, s3k_mk_pmp(uart_addr, S3K_MEM_RW));
	s3k_cap_derive(2, 18, s3k_mk_pmp(uart_addr, S3K_MEM_RW));
	s3k_pmp_load(16, 1);
	s3k_sync();
	alt_puts("hello, world from app0");

	s3k_cap_derive(1, 17, s3k_mk_pmp(app1_addr, S3K_MEM_RWX));
	s3k_mon_cap_move(8, 0, 17, 1, 0);
	s3k_mon_cap_move(8, 0, 18, 1, 1);
	s3k_cap_derive(4, 19, s3k_mk_time(0, 0, 15));
	s3k_mon_cap_move(8, 0, 19, 1, 2);
	s3k_mon_pmp_load(8, 1, 0, 0);
	s3k_mon_pmp_load(8, 1, 1, 1);
	s3k_mon_reg_write(8, 1, 0, 0x80020000);
	s3k_mon_resume(8, 1);
}
