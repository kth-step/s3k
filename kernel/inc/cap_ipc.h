#pragma once

#include "cap_table.h"
#include "error.h"
#include "proc.h"

#include <stdint.h>

typedef struct ipc_msg {
	uint64_t *data_buf;
	cte_t cap_buf;
	bool send_cap;
} ipc_msg_t;

err_t cap_sock_send(proc_t *p, cte_t sock, bool send_cap, reg_t *yield_to);
err_t cap_sock_sendrecv(proc_t *p, cte_t sock, bool send_cap, reg_t *yield_to);
void cap_sock_clear(cap_t cap, proc_t *p);
