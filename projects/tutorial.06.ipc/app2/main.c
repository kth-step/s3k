#include "s3k.h"

#include <stdio.h>

// Main server loop for IPC (Inter-Process Communication)
// Receives a server endpoint as argument
int main(s3k_word_t server)
{
	s3k_msg_t msg = {};
	// Minimal service time

	while (1) {
		// Record cycle count before receiving
		s3k_ipc_replyrecv(server, &msg);
		printf("2> received %d\n", msg.data[0]);
		msg.data[0] = msg.data[0] * msg.data[0];
	}
}
