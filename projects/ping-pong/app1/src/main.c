#include "altio/altio.h"
#include "s3k/s3k.h"

#include <stdint.h>

void *memcpy(void *dest, const void *src, size_t n)
{
	for (int i = 0; i < n; ++i) {
		((char *)dest)[i] = ((char *)src)[i];
	}
	return dest;
}

int main(void)
{
	s3k_msg_t msg;
	s3k_reply_t reply;
	memcpy(msg.data, "ping", 5);
	while (1) {
		do {
			reply = s3k_sock_sendrecv(3, &msg);
			if (reply.err == S3K_ERR_TIMEOUT)
				alt_puts("timeout");
		} while (reply.err);
		alt_puts((char *)reply.data);
	}
}
