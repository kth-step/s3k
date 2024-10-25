#include "altc/altio.h"
#include "altc/string.h"
#include "s3k/s3k.h"

#include "../../tutorial-commons/utils.h"


int main(void)
{
	// Setup UART access
	setup_uart();

	alt_puts("starting app0");

	// Setup app1 capabilities and PC
	setup_app_1();

	// setup_scheduling(PARALLEL); // with parallel no tim-out
	setup_scheduling(ROUND_ROBIN);

	// Setup socket capabilities.
	uint32_t socket = setup_socket(true, true, false);

	// Resume app1
	s3k_mon_resume(MONITOR, APP1_PID);

	s3k_msg_t msg;
	s3k_reply_t reply;
	memcpy(msg.data, "pong", 5);

	s3k_reg_write(S3K_REG_SERVTIME, 4500);
	volatile int x;
	while (1) {
		do {
			reply = s3k_sock_sendrecv(socket, &msg);
			if (reply.err == S3K_ERR_TIMEOUT)
				alt_puts("0> timeout");
		} while (reply.err);
		for (int i=0; i<100; i++) {
			x += 1;
		}
		alt_puts((char *)reply.data);
	}
}
