#pragma once

#include "cap_table.h"
#include "error.h"
#include "proc.h"

#include <stdint.h>

typedef struct ipc_msg {
	cte_t cap_buf;
	bool send_cap;
	uint64_t data[4];
	uint64_t serv_time;
} ipc_msg_t;

err_t cap_sock_send(proc_t *p, cte_t sock, const ipc_msg_t *msg, proc_t **next);
err_t cap_sock_sendrecv(proc_t *p, cte_t sock, const ipc_msg_t *msg,
			proc_t **next);
void cap_sock_clear(cap_t cap, proc_t *p);
