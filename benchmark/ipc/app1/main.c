#include "altc/altio.h"
#include "altc/string.h"
#include "s3k/s3k.h"

#include <stdint.h>

#define csrr(reg)                                                \
	({                                                       \
		register uint64_t __ret;                         \
		__asm__ volatile("csrr %0," #reg : "=r"(__ret)); \
		__ret;                                           \
	})

void measure(uint64_t res[2])
{
	s3k_msg_t msg = {0};
	s3k_reply_t reply;
	uint64_t start, end;
retry:
	do {
#if defined(SCENARIO_CAP)
		msg.send_cap = 1;
		msg.cap_idx = 16;
#elif defined(SCENARIO_TIME)
		msg.send_cap = 1;
		msg.cap_idx = 2;
#endif
		start = csrr(cycle);
		reply = s3k_sock_sendrecv(3, &msg);
		end = csrr(cycle);
	} while (reply.err);
	uint64_t call = reply.data[0] - start;
	uint64_t sendrecv = end - reply.data[0];
	if (call > 4000 || sendrecv > 4000)
		goto retry;
	res[0] = call;
	res[1] = sendrecv;
}

int main(void)
{
	uint64_t res[2];
	alt_puts("starting app1");

	for (int i = 0; i < WARMUP; i++)
		measure(res);
	for (int i = 0; i < MEASUREMENTS; i++) {
		measure(res);
		alt_printf("%d\t%D\t%D\n", i + 1, res[0], res[1]);
	}
}
