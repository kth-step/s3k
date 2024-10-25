#include "altc/altio.h"
#include "altc/string.h"
#include "s3k/s3k.h"

#include "../../tutorial-commons/utils.h"


#include <stdint.h>

int count = 0;

s3k_msg_t on_msg(s3k_reply_t input, uint32_t cap_idx) {
	alt_printf("APP1: %s\n", (char *)input.data);
	s3k_msg_t output;

	volatile int x;
	for (int i=0; i<1000; i++) {
		x += 1;
	}
	alt_snprintf((char*)(& output.data[0]), 10, "pong %d", count++);
	return output;
}

int main(void)
{
	alt_puts("starting app1");
	server_main_loop(on_msg, false);
}
