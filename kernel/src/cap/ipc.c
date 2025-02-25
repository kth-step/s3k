#include "cap/ipc.h"

#include "altc/string.h"
#include "altc/time.h"
#include "cap/ops.h"
#include "cap/table.h"
#include "csr.h"
#include "error.h"
#include "kassert.h"
#include "kernel.h"
#include "macro.h"
#include "proc.h"

#include <stdint.h>

#define SERVER 0
#define CLIENT 1

struct {
	proc_t *server;
	proc_t *client;
	cte_t cap_buf;
} channels[NCHAN];

static err_t do_send(cap_t cap, const ipc_msg_t *msg, proc_t **next)
{
	bool is_server = (cap.sock.tag == 0);
	cte_t cap_buf = channels[cap.sock.chan].cap_buf;

	uint64_t curr_time = time_get();
	uint64_t timeout = timeout_get(csrr(mhartid));

	if (curr_time >= timeout) {
		return ERR_PREEMPTED;
	}

	proc_t *recv;

	if (is_server) {
		recv = channels[cap.sock.chan].client;
		if (!recv)
			return ERR_NO_RECEIVER;
		channels[cap.sock.chan].client = NULL;
	} else {
		recv = channels[cap.sock.chan].server;
		if (!recv)
			return ERR_NO_RECEIVER;
		if (cap.sock.mode == IPC_YIELD
		    && curr_time + recv->regs[REG_SERVTIME] >= timeout)
			return ERR_NO_RECEIVER;
	}

	if (proc_ipc_acquire(recv, cap.sock.chan)) {
		recv->regs[REG_T0] = SUCCESS;
		recv->regs[REG_A0] = cap.sock.tag;
		recv->regs[REG_A2] = msg->data[0];
		recv->regs[REG_A3] = msg->data[1];
		recv->regs[REG_A4] = msg->data[2];
		recv->regs[REG_A5] = msg->data[3];

		if (msg->send_cap) {
			recv->regs[REG_A1] = cte_cap(msg->cap_buf).raw;
			cap_move(msg->cap_buf, cap_buf);
		} else {
			recv->regs[REG_A1] = 0;
		}

		if (cap.sock.mode == IPC_YIELD) {
			recv->timeout = (*next)->timeout;
			*next = recv;
		} else if (cap.sock.mode == IPC_NOYIELD) {
			recv->timeout = 0;
			proc_release(recv);
		} else {
			KASSERT(0);
		}
		return SUCCESS;
	}
	return ERR_NO_RECEIVER;
}

static void do_recv(cap_t cap, cte_t cap_buf, proc_t *recv)
{
	bool is_server = (cap.sock.tag == 0);
	if (is_server) {
		channels[cap.sock.chan].server = recv;
		channels[cap.sock.chan].cap_buf = cap_buf;
		recv->timeout = UINT64_MAX;
		proc_ipc_wait(recv, cap.sock.chan);
	} else {
		channels[cap.sock.chan].client = recv;
		channels[cap.sock.chan].cap_buf = cap_buf;
		if (cap.sock.mode == IPC_NOYIELD)
			recv->timeout = UINT64_MAX;
		proc_ipc_wait(recv, cap.sock.chan);
	}
}

static err_t reply(cte_t sock, cap_t cap, const ipc_msg_t *msg, proc_t **next)
{
	if (msg->send_cap && !(cap.sock.perm & IPC_SCAP))
		return ERR_INVALID_SOCKET;
	return do_send(cap, msg, next);
}

static err_t send(cte_t sock, cap_t cap, const ipc_msg_t *msg, proc_t **next)
{
	if (msg->send_cap && !(cap.sock.perm & IPC_CCAP))
		return ERR_INVALID_SOCKET;
	return do_send(cap, msg, next);
}

static err_t replyrecv(cte_t sock, cap_t cap, const ipc_msg_t *msg,
		       proc_t **next)
{
	cte_t cap_buf = msg->cap_buf;
	proc_t *server = *next;
	KASSERT(server->state == PSF_BUSY);

	// Can send capability?
	if (msg->send_cap && !(cap.sock.perm & IPC_SCAP))
		return ERR_INVALID_SOCKET;

	// Can receive capability?
	if ((cap.sock.perm & IPC_CCAP) && !cte_is_empty(cap_buf)
	    && !msg->send_cap)
		return ERR_DST_OCCUPIED;

	err_t err = do_send(cap, msg, next);
	if (err == ERR_PREEMPTED) {
		*next = NULL;
		return err;
	} else if (err == ERR_NO_RECEIVER && msg->send_cap) {
		cap_delete(msg->cap_buf);
	}
	do_recv(cap, cap_buf, server);
	if (*next == server)
		*next = NULL;
	return ERR_TIMEOUT;
}

static err_t call(cte_t sock, cap_t cap, const ipc_msg_t *msg, proc_t **next)
{
	cte_t cap_buf = msg->cap_buf;
	proc_t *client = *next;

	// Can send capability?
	if (msg->send_cap && !(cap.sock.perm & IPC_CCAP))
		return ERR_INVALID_SOCKET;

	// Can receive capability?
	if ((cap.sock.perm & IPC_SCAP) && !cte_is_empty(cap_buf)
	    && !msg->send_cap)
		return ERR_DST_OCCUPIED;

	err_t err = do_send(cap, msg, next);
	if (err)
		return err;
	do_recv(cap, cap_buf, client);
	if (*next == client)
		*next = NULL;
	return ERR_TIMEOUT;
}

/* Entry points */
err_t cap_sock_send(cte_t sock, const ipc_msg_t *msg, proc_t **next)
{
	cap_t cap = cte_cap(sock);
	if (cap.type == CAPTY_NONE)
		return ERR_EMPTY;
	if (cap.type != CAPTY_SOCKET)
		return ERR_INVALID_SOCKET;
	if ((*next)->state & PSF_SUSPENDED) {
		*next = NULL;
		return ERR_PREEMPTED;
	}
	if (cap.sock.tag == 0) {
		return reply(sock, cap, msg, next);
	} else {
		return send(sock, cap, msg, next);
	}
}

err_t cap_sock_recv(cte_t sock, cte_t cap_buf, proc_t **next)
{
	cap_t cap = cte_cap(sock);
	if (cap.type == CAPTY_NONE)
		return ERR_EMPTY;
	if (cap.type != CAPTY_SOCKET)
		return ERR_INVALID_CAPABILITY;
	if ((cap.sock.perm & IPC_CCAP) && !cte_is_empty(cap_buf))
		return ERR_DST_OCCUPIED;
	if ((*next)->state & PSF_SUSPENDED) {
		*next = NULL;
		return ERR_PREEMPTED;
	}

	if (cap.sock.tag == 0) {
		// server
		do_recv(cap, cap_buf, *next);
		*next = NULL;
		return ERR_TIMEOUT;
	} else if (channels[cap.sock.chan].client == *next) {
		proc_t *server = channels[cap.sock.chan].server;
		proc_t *client = *next;
		if (server && cap.sock.mode == IPC_YIELD
		    && proc_acquire(server)) {
			proc_ipc_wait(client, cap.sock.chan);
			channels[cap.sock.chan].cap_buf = cap_buf;
			server->timeout = (*next)->timeout;
			*next = server;
		}
		return ERR_TIMEOUT;
	}
	return ERR_INVALID_STATE;
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
		return ERR_PREEMPTED;
	}
	KASSERT((*next)->state == PSF_BUSY);

	if (cap.sock.tag == 0) {
		return replyrecv(sock, cap, msg, next);
	} else {
		return call(sock, cap, msg, next);
	}
}

void cap_sock_clear(cap_t cap, proc_t *p)
{
	if (cap.sock.tag == 0) {
		channels[cap.sock.chan].server = NULL;
		channels[cap.sock.chan].client = NULL;
		channels[cap.sock.chan].cap_buf = NULL;
	} else if (channels[cap.sock.chan].client == p) {
		channels[cap.sock.chan].client = NULL;
	}
}
