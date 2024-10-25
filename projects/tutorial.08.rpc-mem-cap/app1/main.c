#include "altc/altio.h"
#include "altc/string.h"
#include "s3k/s3k.h"

#include "../../tutorial-commons/utils.h"


#include <stdint.h>

int count = 0;

s3k_msg_t on_msg(s3k_reply_t input, uint32_t cap_idx) {
	alt_printf("1> handling request\n");
	s3k_msg_t output;
	if (input.cap.type == 0) {
		alt_printf("1> missing capability in message\n");
		alt_snprintf((char*)&(output.data), 10, "");
		return output;
	}
    
	s3k_pmp_load(cap_idx, APP_1_PMP_SLOT_BUFFER);
	s3k_sync_mem();
	uint32_t counter = *((uint32_t *)SHARED_BUFFER_BASE);
	alt_snprintf((char*)&(output.data), 10, "%d", counter);
	return output;
}

int main(void)
{
	alt_puts("starting app1");
	server_main_loop(on_msg, true);
}
