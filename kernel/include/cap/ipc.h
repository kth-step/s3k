#pragma once

#include "cap/table.h"
#include "error.h"
#include "proc.h"

#include <stdint.h>

typedef struct ipc_msg {
	cte_t cap_buf;
	bool send_cap;
	uint64_t data[4];
} ipc_msg_t;

err_t cap_sock_send(cte_t sock, const ipc_msg_t *msg, proc_t **next);
err_t cap_sock_recv(cte_t sock, const cte_t cap_buf, proc_t **next);
err_t cap_sock_sendrecv(cte_t sock, const ipc_msg_t *msg, proc_t **next);
void cap_sock_clear(cap_t cap, proc_t *next);
