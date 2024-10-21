#include "altc/altio.h"
#include "altc/string.h"
#include "s3k/s3k.h"

#include <stdint.h>

int main(void)
{
	alt_puts("starting app1");
	s3k_msg_t msg;
	s3k_reply_t reply;
	memcpy(msg.data, "ping", 5);
	while (1) {
		do {
			reply = s3k_sock_sendrecv(3, &msg);
			while (reply.err == S3K_ERR_TIMEOUT) {
				alt_puts("timeout");
				reply = s3k_sock_recv(3, msg.cap_idx);
			}
		} while (reply.err);
		alt_puts((char *)reply.data);
	}
}
