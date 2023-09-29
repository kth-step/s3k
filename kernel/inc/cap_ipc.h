#pragma once

#include "cap_table.h"
#include "error.h"
#include "proc.h"

#include <stdint.h>

typedef struct ipc_msg {
	uint64_t buf[4];
	cte_t cbuf;
	bool send_cap;
} ipc_msg_t;

err_t cap_sock_send(proc_t *p, cte_t c, ipc_msg_t *msg, reg_t *yield_to);
err_t cap_sock_call(proc_t *p, cte_t c, ipc_msg_t *msg, reg_t *yield_to);
err_t cap_sock_reply(proc_t *p, cte_t c, ipc_msg_t *msg, reg_t *yield_to);
err_t cap_sock_replyrecv(proc_t *p, cte_t c, ipc_msg_t *msg, reg_t *yield_to);
err_t cap_sock_recv(proc_t *p, cte_t c, ipc_msg_t *msg);
