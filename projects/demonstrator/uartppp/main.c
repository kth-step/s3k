#include "../config.h"
#include "altc/altio.h"
#include "s3k/s3k.h"

#define PPP_ESC 0x7C
#define PPP_BGN 0x7B
#define PPP_END 0x7D

void ppp_send(char *buf, size_t len);
size_t ppp_recv(char *buf);

void main(void)
{
	s3k_cap_revoke(0x2);
	alt_puts("{uart started}");
	s3k_msg_t msg;
	s3k_reply_t reply;

	char *buf = SHARED0_MEM;

	while (1) {
		size_t len = ppp_recv(buf);
		alt_puts("{received message}");
		msg.data[0] = len;
		reply = s3k_sock_sendrecv(0x5, &msg);
		if (reply.err != S3K_SUCCESS) {
			char *s = "error0";
			if (reply.err < 10)
				s[5] = '0' + reply.err;
			else
				s[5] = 'A' + reply.err - 10;
			ppp_send(s, 6);
			continue;
		}
		ppp_send(buf, reply.data[0]);
	}
}

void ppp_send(char *buf, size_t len)
{
	alt_putchar(PPP_BGN);
	for (int i = 0; i < len; ++i) {
		char c = buf[i];
		switch (c) {
		case PPP_BGN:
		case PPP_END:
		case PPP_ESC:
			alt_putchar(PPP_ESC);
			c ^= 0x20;
			/* fallthrough */
		default:
			alt_putchar(c);
		}
	}
	alt_putchar(PPP_END);
}

size_t ppp_recv(char *buf)
{
	size_t i = 0;
	while (alt_getchar() != PPP_BGN)
		;
	while (1) {
		char c = alt_getchar();
		switch (c) {
		case PPP_BGN:
			i = 0;
			break;
		case PPP_END:
			return i;
		case PPP_ESC:
			c = alt_getchar() ^ 0x20;
			/* fallthrough */
		default:
			buf[i++] = c;
		}
	}
}
