#include "altc/altio.h"
#include "altc/string.h"
#include "s3k/s3k.h"

#include "../../tutorial-commons/utils.h"

uint32_t setup_buffer_cap() {
	uint32_t new_cap = find_free_cap();
	debug_capability_from_idx(RAM_MEM);
	log_sys("app 0: derive cap for shared buffer", 
		s3k_cap_derive(RAM_MEM, new_cap,
           s3k_mk_memory(SHARED_BUFFER_BASE, SHARED_BUFFER_BASE + SHARED_BUFFER_SIZE, S3K_MEM_RW)));
	return new_cap;
}

uint32_t setup_buffer_pmp(uint32_t parent_cap_idx) {
	uint32_t pmp_cap_idx = find_free_cap();
	log_sys("app 0: revoke parent cap", s3k_cap_revoke(parent_cap_idx));
	log_sys("app 0: derive new mem cap",  
		s3k_cap_derive(parent_cap_idx, pmp_cap_idx,
                        s3k_mk_pmp( s3k_napot_encode(SHARED_BUFFER_BASE, SHARED_BUFFER_SIZE), S3K_MEM_RW)));
    log_sys("app 0: load pmp for new buffer", s3k_pmp_load(pmp_cap_idx, BUFFER_PMP));
	s3k_sync_mem();
	return pmp_cap_idx;
}

int main(void)
{
	// Setup UART access
	setup_uart();

	alt_puts("starting app0");

	// Setup app1 capabilities and PC
	setup_app_1();

	// Setup socket capabilities.
	uint64_t socket = setup_socket(false, true, true);

	// Resume app1
	s3k_mon_resume(MONITOR, APP1_PID);

	wait_for_app1_blocked();
	alt_printf("APP0: other app booted\n");

	uint32_t parent_cap = setup_buffer_cap();

	s3k_msg_t msg;
	s3k_reply_t reply;
	s3k_reg_write(S3K_REG_SERVTIME, 4500);

	uint32_t counter = 0;
	while (1) {
		uint32_t child_cap = setup_buffer_pmp(parent_cap);
		alt_printf("APP0: writing counter in shared memory  %d\n", counter);
		*((uint32_t *) SHARED_BUFFER_BASE) = ++counter;
		msg.send_cap = true;
		msg.cap_idx = child_cap;
		alt_printf("APP0: sending %d\n", counter);
		reply = send_receive_forever(socket, msg);
		alt_printf("APP0: received %s\n", (char *)reply.data);
	}
}
