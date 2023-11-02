#include "cap_ipc.h"

#include "cap_ops.h"
#include "cap_table.h"
#include "csr.h"
#include "drivers/time.h"
#include "error.h"
#include "kassert.h"
#include "proc.h"

#include <stdint.h>

#define SERVER 0
#define CLIENT 1

static proc_t *clients[S3K_CHAN_CNT];
static proc_t *servers[S3K_CHAN_CNT];

void set_client(uint64_t chan, proc_t *proc, ipc_mode_t mode)
{
	// If no yielding mode, we have no timeout.
	if (mode == IPC_NOYIELD)
		proc->timeout = UINT64_MAX;
	else
		proc->timeout = timeout_get(csrr_mhartid());
	proc->serv_time = 0;
	clients[chan] = proc;
	proc_ipc_wait(proc, chan);
}

void set_server(uint64_t chan, proc_t *proc, ipc_mode_t mode)
{
	if (mode == IPC_NOYIELD)
		proc->serv_time = 0;
	else
		proc->serv_time = proc->regs[REG_SERVTIME];
	proc->timeout = UINT64_MAX;
	servers[chan] = proc;
	proc_ipc_wait(proc, chan);
}

err_t valid_sock(cap_t sock_cap, bool send_cap)
{
	if (!sock_cap.type)
		return ERR_EMPTY;
	if (sock_cap.type != CAPTY_SOCKET)
		return ERR_INVALID_SOCKET;

	// If send_cap == true, then can_send_cap must be true.
	bool is_server = (sock_cap.sock.tag == 0);
	ipc_perm_t perm = sock_cap.sock.perm;
	bool can_send_cap = (perm & (is_server ? IPC_SCAP : IPC_CCAP));
	if (!can_send_cap && send_cap)
		return ERR_INVALID_SOCKET;
	return SUCCESS;
}

err_t do_sock_send(cap_t sock_cap, const ipc_msg_t *msg, proc_t **next)
{
	uint64_t chan = sock_cap.sock.chan;
	uint64_t tag = sock_cap.sock.tag;
	uint64_t mode = sock_cap.sock.mode;
	uint64_t perm = sock_cap.sock.perm;
	bool is_server = (tag == 0);

	proc_t *recv = is_server ? clients[chan] : servers[chan];

	bool send_data = (perm & (is_server ? IPC_SDATA : IPC_CDATA));

	if (!recv || !proc_ipc_acquire(recv, chan))
		return ERR_NO_RECEIVER;

	if (is_server)
		clients[chan] = NULL;
	else
		servers[chan] = NULL;

	recv->regs[REG_T0] = SUCCESS;
	recv->regs[REG_A0] = tag;
	recv->regs[REG_A1] = 0;
	if (send_data) {
		recv->regs[REG_A2] = msg->data[0];
		recv->regs[REG_A3] = msg->data[0];
		recv->regs[REG_A4] = msg->data[0];
		recv->regs[REG_A5] = msg->data[0];
	}
	if (msg->send_cap) {
		cap_move(msg->src_buf, recv->cap_buf,
			 (cap_t *)&recv->regs[REG_A1]);
	}

	if (mode == IPC_YIELD) {
		// Yield to receiver
		*next = recv;
		return YIELD;
	} else {
		// No yield, just success
		proc_release(recv);
		return SUCCESS;
	}
}

err_t do_sock_recv(proc_t *recv, cap_t sock_cap)
{
	chan_t chan = sock_cap.sock.chan;
	ipc_mode_t mode = sock_cap.sock.mode;
	ipc_perm_t perm = sock_cap.sock.perm;
	uint32_t tag = sock_cap.sock.tag;
	bool is_server = (tag == 0);

	// If the other party can send a capability.
	bool recv_cap = perm & (is_server ? IPC_CCAP : IPC_SCAP);

	// if we can receive a capability, free the slot.
	if (recv_cap)
		cap_delete(recv->cap_buf);

	if (is_server)
		set_server(chan, recv, mode);
	else
		set_client(chan, recv, mode);
	return YIELD;
}

err_t cap_sock_send(cte_t sock, const ipc_msg_t *msg, proc_t **next)
{
	cap_t sock_cap = cte_cap(sock);
	proc_t *proc = proc_get(cte_pid(sock));

	// Check that we have a valid socket capability.
	err_t err = valid_sock(sock_cap, msg->send_cap);
	if (err)
		return err;

	// If suspend flag is set, suspend.
	if (proc->state & PSF_SUSPENDED)
		return ERR_SUSPENDED;
	return do_sock_send(sock_cap, msg, next);
}

err_t cap_sock_recv(cte_t sock)
{
	cap_t sock_cap = cte_cap(sock);
	proc_t *proc = proc_get(cte_pid(sock));

	err_t err = valid_sock(sock_cap, false);
	if (err)
		return err;
	if (sock_cap.sock.tag != 0)
		return ERR_INVALID_SOCKET;

	// If suspend flag is set, suspend.
	if (proc->state & PSF_SUSPENDED)
		return ERR_SUSPENDED;
	return do_sock_recv(proc, sock_cap);
}

err_t cap_sock_sendrecv(cte_t sock, const ipc_msg_t *msg, proc_t **next)
{
	cap_t sock_cap = cte_cap(sock);
	proc_t *proc = proc_get(cte_pid(sock));

	err_t err = valid_sock(sock_cap, msg->send_cap);
	if (err)
		return err;

	// If suspend flag is set, suspend.
	if (proc->state & PSF_SUSPENDED)
		return ERR_SUSPENDED;

	// Try send capability
	err = do_sock_send(sock_cap, msg, next);

	bool is_server = (sock_cap.sock.tag == 0);

	if (!is_server && err == ERR_NO_RECEIVER) {
		// Clients fail if we get an error, servers do not care.
		return ERR_NO_RECEIVER;
	} else if (is_server && msg->send_cap) {
		// Servers scrap capabilities that were not sent.
		cap_delete(proc->cap_buf);
	}
	return do_sock_recv(proc, sock_cap);
}

void cap_sock_clear(cap_t cap, proc_t *p)
{
	if (!cap.sock.tag) {
		servers[cap.sock.chan] = NULL;
	} else if (clients[cap.sock.chan] == p) {
		clients[cap.sock.chan] = NULL;
	}
}
