#include "altc/altio.h"
#include "s3k/s3k.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

int main(void)
{
	s3k_msg_t msg;
	do {
		msg.cap_idx = 10;
		s3k_sock_sendrecv(3, &msg);
	} while (1);
}
