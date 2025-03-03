#include <altc/altio.h>
#include <s3k/s3k.h>

void main(void)
{
	s3k_cap_derive(2, 10,
		       s3k_mk_pmp(s3k_napot_encode(0x10000000, 0x20), 0x3));
	s3k_pmp_load(10, 1);
	s3k_sync();
	alt_putstr("hello, world\r\n");
}
