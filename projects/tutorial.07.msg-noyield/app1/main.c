#include "altc/altio.h"
#include "altc/string.h"
#include "s3k/s3k.h"

#include <stdint.h>

#include "../../tutorial-commons/utils.h"

int main(void)
{
	alt_puts("starting app1");
	s3k_msg_t msg;
	s3k_reply_t reply;
	int count = 0;
	volatile int x;
	while (1) {
		do {
			alt_snprintf((char*)&(msg.data), 32, "ping %d", count);
			reply = s3k_sock_sendrecv(APP_1_CAP_SOCKET, &msg);
			if (reply.err == S3K_ERR_TIMEOUT)
				alt_puts("1> timeout");
		} while (reply.err);
		alt_puts((char *)reply.data);
		count++;
		for (int i=0; i<1000000; i++) {
			x += 1;
		}	
	}
}
