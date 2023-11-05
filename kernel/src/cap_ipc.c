#include "cap_ipc.h"

#include "altc/string.h"
#include "cap_ops.h"
#include "cap_table.h"
#include "csr.h"
#include "drivers/time.h"
#include "error.h"
#include "kassert.h"
#include "macro.h"
#include "proc.h"

#include <stdint.h>

#define SERVER 0
#define CLIENT 1

static proc_t *clients[S3K_CHAN_CNT];
static proc_t *servers[S3K_CHAN_CNT];

static err_t _cap_sock_reply(cte_t sock, cap_t cap, const ipc_msg_t *msg,
			     proc_t **next);
static err_t _cap_sock_send(cte_t sock, cap_t cap, const ipc_msg_t *msg,
			    proc_t **next);
static err_t _cap_sock_recv(cte_t sock, cap_t cap, proc_t **next);
static err_t _cap_sock_replyrecv(cte_t sock, cap_t cap, const ipc_msg_t *msg,
				 proc_t **next);
static err_t _cap_sock_call(cte_t sock, cap_t cap, const ipc_msg_t *msg,
			    proc_t **next);

err_t _cap_sock_reply(cte_t sock, cap_t cap, const ipc_msg_t *msg,
		      proc_t **next)
{
	proc_t *client = clients[cap.sock.chan];
	if (client && proc_ipc_acquire(client, cap.sock.chan)) {
		if (msg->send_cap) {
			cap_move((*next)->cap_buf, client->cap_buf,
				 (cap_t *)&client->regs[REG_A1]);
		} else {
			client->regs[REG_A1] = 0;
		}
		memcpy(&client->regs[REG_A0], msg->data, ARRAY_SIZE(msg->data));

		if (cap.sock.mode == IPC_YIELD) {
			*next = client;
		} else {
			proc_release(client);
		}
		return SUCCESS;
	} else if (msg->send_cap) {
		cap_delete((*next)->cap_buf);
	}

	return ERR_NO_RECEIVER;
}

err_t _cap_sock_send(cte_t sock, cap_t cap, const ipc_msg_t *msg, proc_t **next)
{
	proc_t *server = servers[cap.sock.chan];
	if (!server || !proc_ipc_acquire(server, cap.sock.chan))
		return ERR_NO_RECEIVER;

	if (msg->send_cap) {
		cap_move((*next)->cap_buf, server->cap_buf,
			 (cap_t *)&server->regs[REG_A1]);
	} else {
		server->regs[REG_A1] = 0;
	}
	memcpy(&server->regs[REG_A0], msg->data, ARRAY_SIZE(msg->data));

	if (cap.sock.mode == IPC_YIELD) {
		*next = server;
	} else {
		proc_release(server);
	}
	return SUCCESS;
}

err_t _cap_sock_recv(cte_t sock, cap_t cap, proc_t **next)
{
	proc_ipc_wait(*next, cap.sock.chan);
	servers[cap.sock.chan] = *next;
	*next = NULL;
	return SUCCESS;
}

err_t _cap_sock_replyrecv(cte_t sock, cap_t cap, const ipc_msg_t *msg,
			  proc_t **next)
{
	proc_ipc_wait(*next, cap.sock.chan);
	servers[cap.sock.chan] = *next;

	proc_t *client = clients[cap.sock.chan];
	proc_t *server = *next;
	*next = NULL;
	if (client && proc_ipc_acquire(client, cap.sock.chan)) {
		client->regs[REG_A0] = cap.sock.tag;
		if (msg->send_cap) {
			cap_move(server->cap_buf, client->cap_buf,
				 (cap_t *)&client->regs[REG_A1]);
		} else {
			client->regs[REG_A1] = 0;
		}
		memcpy(&client->regs[REG_A2], msg->data, 4);
		if (cap.sock.mode == IPC_YIELD) {
			*next = client;
		} else {
			proc_release(client);
		}
	}

	server->timeout = UINT64_MAX;
	if (msg->send_cap)
		cap_delete((*next)->cap_buf);
	return SUCCESS;
}

err_t _cap_sock_call(cte_t sock, cap_t cap, const ipc_msg_t *msg, proc_t **next)
{
	proc_t *client = *next;
	proc_t *server = servers[cap.sock.chan];
	if (!server || !proc_ipc_acquire(server, cap.sock.chan))
		return ERR_NO_RECEIVER;

	server->regs[REG_A0] = cap.sock.tag;
	if (msg->send_cap) {
		cap_move(client->cap_buf, server->cap_buf,
			 (cap_t *)&server->regs[REG_A1]);
	} else {
		server->regs[REG_A1] = 0;
	}
	if (cap.sock.perm & IPC_CDATA)
		memcpy(&server->regs[REG_A2], msg->data, 4);

	proc_ipc_wait(client, cap.sock.chan);
	clients[cap.sock.chan] = client;
	if (cap.sock.mode == IPC_YIELD) {
		client->timeout = timeout_get(csrr_mhartid());
		*next = server;
	} else {
		client->timeout = UINT64_MAX;
		*next = NULL;
		proc_release(server);
	}
	return SUCCESS;
}

/* Entry points */
err_t cap_sock_send(cte_t sock, const ipc_msg_t *msg, proc_t **next)
{
	cap_t cap = cte_cap(sock);
	if (cap.type == CAPTY_NONE)
		return ERR_EMPTY;
	if (cap.type != CAPTY_SOCKET)
		return ERR_INVALID_CAPABILITY;
	if ((*next)->state & PSF_SUSPENDED) {
		*next = NULL;
		return ERR_SUSPENDED;
	}
	if (cap.sock.tag == 0) {
		// Are we allowed to send a capability?
		if (msg->send_cap && !(cap.sock.perm & IPC_SCAP))
			return ERR_INVALID_SOCKET;
		return _cap_sock_reply(sock, cap, msg, next);
	}
	// Are we allowed to send a capability?
	if (msg->send_cap && !(cap.sock.perm & IPC_CCAP))
		return ERR_INVALID_SOCKET;
	return _cap_sock_send(sock, cap, msg, next);
}

err_t cap_sock_recv(cte_t sock, proc_t **next)
{
	cap_t cap = cte_cap(sock);
	if (cap.type == CAPTY_NONE)
		return ERR_EMPTY;
	if (cap.type != CAPTY_SOCKET)
		return ERR_INVALID_CAPABILITY;
	if (cap.sock.tag != 0)
		return ERR_INVALID_SOCKET;
	if ((cap.sock.perm & IPC_CCAP) && !cte_is_empty((*next)->cap_buf))
		return ERR_DST_OCCUPIED;
	if ((*next)->state & PSF_SUSPENDED) {
		*next = NULL;
		return ERR_SUSPENDED;
	}
	return _cap_sock_recv(sock, cap, next);
}

err_t cap_sock_sendrecv(cte_t sock, const ipc_msg_t *msg, proc_t **next)
{
	cap_t cap = cte_cap(sock);
	if (cap.type == CAPTY_NONE)
		return ERR_EMPTY;
	if (cap.type != CAPTY_SOCKET)
		return ERR_INVALID_CAPABILITY;
	if ((*next)->state & PSF_SUSPENDED) {
		*next = NULL;
		return ERR_SUSPENDED;
	}

	if (cap.sock.tag == 0) {
		// SERVER CASE
		// Are we allowed to send a capability?
		if (msg->send_cap && !(cap.sock.perm & IPC_SCAP))
			return ERR_INVALID_SOCKET;
		// Can we receive a capability?
		if ((cap.sock.perm & IPC_CCAP)
		    && !cte_is_empty((*next)->cap_buf) && !msg->send_cap)
			return ERR_DST_OCCUPIED;
		return _cap_sock_replyrecv(sock, cap, msg, next);
	}

	// CLIENT CASE
	if (msg->send_cap && !(cap.sock.perm & IPC_CCAP))
		return ERR_INVALID_SOCKET;
	// Can we receive a capability?
	if ((cap.sock.perm & IPC_SCAP) && !cte_is_empty((*next)->cap_buf)
	    && !msg->send_cap)
		return ERR_DST_OCCUPIED;
	return _cap_sock_call(sock, cap, msg, next);
}

void cap_sock_clear(cap_t cap, proc_t *p)
{
	if (!cap.sock.tag) {
		servers[cap.sock.chan] = NULL;
	} else if (clients[cap.sock.chan] == p) {
		clients[cap.sock.chan] = NULL;
	}
}
