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

	// Setup socket capabilities.
	uint64_t socket = setup_socket(false, true, false);

	// Resume app1
	s3k_mon_resume(MONITOR, APP1_PID);

	wait_for_app1_blocked();

	s3k_msg_t msg;
	s3k_reply_t reply;
	memcpy(msg.data, "ping", 5);

	s3k_reg_write(S3K_REG_SERVTIME, 4500);
	while (1) {
		reply = send_receive_forever(socket, msg);
		alt_printf("APP0: %s\n", (char *)reply.data);
	}
}
