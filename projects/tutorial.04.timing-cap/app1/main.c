#include "altc/altio.h"
#include "s3k/s3k.h"

#include "../../tutorial-commons/utils.h"

int main(void)
{
	alt_puts("hello, world from app1");
	s3k_cap_revoke(APP_1_TIME);
	while(1)
			alt_puts("1");
	alt_puts("bye from app1");
}
