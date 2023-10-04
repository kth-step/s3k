#include "cap_ipc.h"

#include "cap_ops.h"
#include "cap_table.h"
#include "error.h"
#include "kassert.h"
#include "proc.h"

#include <stdint.h>

#define SERVER 0
#define CLIENT 1

static proc_t *receivers[S3K_CHAN_CNT][2];

static proc_t *_get_client(uint64_t chan);
static void _set_client(uint64_t chan, proc_t *client, ipc_mode_t mode);
static proc_t *_get_server(uint64_t chan);
static void _set_server(uint64_t chan, proc_t *server, ipc_mode_t mode);
static err_t _validate_socket(cap_t cap, bool send_cap);

proc_t *_get_client(uint64_t chan)
{
	proc_t *p = receivers[chan][CLIENT];
	if (p && proc_ipc_acquire(p, chan))
		return p;
	return NULL;
}

void _set_client(uint64_t chan, proc_t *proc, ipc_mode_t mode)
{
	proc_ipc_wait(proc, chan);
	// If no yielding mode, we have no timeout.
	if (mode == IPC_NOYIELD)
		proc->timeout = UINT64_MAX;
	proc->service_time = 0;
	receivers[chan][CLIENT] = proc;
}

proc_t *_get_server(uint64_t chan)
{
	proc_t *p = receivers[chan][SERVER];
	if (p && proc_ipc_acquire(p, chan))
		return p;
	return NULL;
}

void _set_server(uint64_t chan, proc_t *proc, ipc_mode_t mode)
{
	proc_ipc_wait(proc, chan);
	if (mode == IPC_NOYIELD)
		proc->service_time = 0;
	proc->timeout = UINT64_MAX;
	receivers[chan][SERVER] = proc;
}

err_t _validate_socket(cap_t cap, bool send_cap)
{
	if (!cap.type)
		return ERR_EMPTY;
	if (cap.type != CAPTY_SOCKET)
		return ERR_INVALID_SOCKET;
	if (!send_cap)
		return SUCCESS;
	if (cap.sock.perm & (cap.sock.tag ? IPC_CCAP : IPC_SCAP))
		return SUCCESS;
	return ERR_INVALID_SOCKET;
}

err_t _cap_sock_send(proc_t *sender, cap_t cap, bool send_cap,
		     uint64_t *yield_to)
{
	uint64_t chan = cap.sock.chan;
	uint64_t tag = cap.sock.tag;
	uint64_t mode = cap.sock.mode;

	proc_t *recv = tag ? _get_server(chan) : _get_client(chan);

	if (!recv)
		return ERR_NO_RECEIVER;

	recv->tf.t0 = SUCCESS;
	recv->tf.a0 = tag;
	recv->tf.a1 = 0;
	recv->tf.a2 = sender->tf.a1;
	recv->tf.a3 = sender->tf.a2;
	recv->tf.a4 = sender->tf.a3;
	recv->tf.a5 = sender->tf.a4;
	if (send_cap)
		cap_move(sender->cap_buf, recv->cap_buf, (cap_t *)&recv->tf.a1);

	if (mode == IPC_YIELD) {
		// Yield to receiver
		*yield_to = (uint64_t)recv;
		return YIELD;
	} else {
		// No yield, just success
		proc_release(recv);
		return SUCCESS;
	}
}

err_t _cap_sock_recv(proc_t *recv, cap_t sock_cap)
{
	uint64_t chan = sock_cap.sock.chan;
	uint64_t tag = sock_cap.sock.tag;
	ipc_mode_t mode = sock_cap.sock.mode;
	bool recv_cap = sock_cap.sock.perm & (tag ? IPC_SCAP : IPC_CCAP);

	// if we can receive a capability, free the slot.
	if (recv_cap)
		cap_delete(recv->cap_buf);

	if (tag) {
		// tag != 0 means client capability
		_set_client(chan, recv, mode);
	} else {
		// tag == 0 means client capability
		_set_server(chan, recv, mode);
	}
	return YIELD;
}

err_t cap_sock_send(proc_t *proc, cte_t sock, bool send_cap, uint64_t *yield_to)
{
	cap_t cap = cte_cap(sock);

	err_t err = _validate_socket(cap, send_cap);
	if (err)
		return err;
	if (proc->state & PSF_SUSPENDED)
		return ERR_SUSPENDED;
	return _cap_sock_send(proc, cap, send_cap, yield_to);
}

err_t cap_sock_sendrecv(proc_t *proc, cte_t sock, bool send_cap,
			uint64_t *yield_to)
{
	cap_t cap = cte_cap(sock);
	err_t err = _validate_socket(cap, send_cap);
	if (err)
		return err;
	if (proc->state & PSF_SUSPENDED)
		return ERR_SUSPENDED;
	err = _cap_sock_send(proc, cap, send_cap, yield_to);

	if (cap.sock.tag && (err == ERR_NO_RECEIVER))
		return err;
	// Delete capability if it should have been sent
	if (send_cap)
		cap_delete(proc->cap_buf);
	_cap_sock_recv(proc, cap);
	return YIELD;
}

void cap_sock_clear(cap_t cap, proc_t *p)
{
	if (!cap.sock.tag) {
		receivers[cap.sock.chan][SERVER] = NULL;
	} else if (receivers[cap.sock.chan][CLIENT] == p) {
		receivers[cap.sock.chan][CLIENT] = NULL;
	}
}
