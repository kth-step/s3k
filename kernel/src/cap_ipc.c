#include "cap_ipc.h"

#include "cap_ops.h"
#include "cap_table.h"
#include "error.h"
#include "kassert.h"
#include "proc.h"

typedef struct {
	proc_t *serv;
	proc_t *clnt;
	cte_t serv_dst, clnt_dst;
} chan_info_t;

static chan_info_t chan_infos[S3K_CHAN_CNT];

static err_t validate_client_cap(cap_t cap, ipc_msg_t *msg, bool recv)
{
	if (!cap.type)
		return ERR_EMPTY;
	if ((cap.type != CAPTY_SOCKET) || (cap.sock.tag == 0))
		return ERR_INVALID_CLIENT;
	// If we try to send capability but not allowed.
	if (!(cap.sock.perm & IPC_CCAP) && msg->send_cap)
		return ERR_INVALID_CLIENT;
	// If we do send capability
	if ((cap.sock.perm & IPC_CCAP) && msg->send_cap)
		return SUCCESS;
	// If we must receive capability but has no space
	if (recv && (cap.sock.perm & IPC_SCAP) && !cte_is_empty(msg->cbuf))
		return ERR_DST_OCCUPIED;
	return SUCCESS;
}

static err_t validate_server_cap(cap_t cap, ipc_msg_t *msg, bool recv)
{
	if (!cap.type)
		return ERR_EMPTY;
	if ((cap.type != CAPTY_SOCKET) || (cap.sock.tag != 0))
		return ERR_INVALID_SERVER;
	// If we try to send capability but not allowed.
	if (!(cap.sock.perm & IPC_SCAP) && msg->send_cap)
		return ERR_INVALID_SERVER;
	// If we do send capability
	if ((cap.sock.perm & IPC_SCAP) && msg->send_cap)
		return SUCCESS;
	// If we must receive capability but has no space
	if (recv && (cap.sock.perm & IPC_CCAP) && !cte_is_empty(msg->cbuf))
		return ERR_DST_OCCUPIED;
	return SUCCESS;
}

static void send_msg(uint64_t *args, cte_t dst, ipc_msg_t *msg, uint64_t tag)
{
	args[0] = tag;
	args[1] = msg->buf[0];
	args[2] = msg->buf[1];
	args[3] = msg->buf[2];
	args[4] = msg->buf[3];
	args[5] = 0;
	args[6]
	    = (msg->send_cap) ? cap_move(msg->cbuf, dst, (cap_t *)&args[5]) : 0;
}

err_t cap_sock_send(proc_t *p, cte_t c, ipc_msg_t *msg, reg_t *yield_to)
{
	cap_t cap = cte_cap(c);

	// check capability type
	err_t err = validate_client_cap(cap, msg, false);
	if (err)
		return err;

	if (proc_is_suspended(p))
		return ERR_SUSPENDED;

	chan_info_t *ci = &chan_infos[cap.sock.chan];

	proc_t *recv = ci->serv;
	cte_t dst = ci->serv_dst;
	if (!recv || proc_ipc_acquire(recv, cap.sock.chan))
		return ERR_NO_RECEIVER;

	// check capability sending
	recv->tf.t0 = SUCCESS;
	send_msg(&recv->tf.a0, dst, msg, cap.sock.tag);

	switch (cap.sock.mode) {
	case IPC_YIELD:
		proc_release(p);
		*yield_to = recv->pid;
		return YIELD;
	case IPC_NOYIELD:
		proc_release(recv);
		return SUCCESS;
	default:
		KASSERT(0);
	}
}

err_t cap_sock_call(proc_t *p, cte_t c, ipc_msg_t *msg, reg_t *yield_to)
{
	cap_t cap = cte_cap(c);

	// check capability type
	err_t err = validate_client_cap(cap, msg, true);
	if (err)
		return err;

	if (proc_is_suspended(p))
		return ERR_SUSPENDED;

	chan_info_t *ci = &chan_infos[cap.sock.chan];

	proc_t *recv = ci->serv;
	cte_t dst = ci->serv_dst;
	if (!recv || proc_ipc_acquire(recv, cap.sock.chan))
		return ERR_NO_RECEIVER;

	// check capability sending
	send_msg(&recv->tf.a0, dst, msg, cap.sock.tag);

	proc_ipc_wait(p, cap.sock.chan);
	ci->clnt = p;
	ci->clnt_dst = (cap.sock.perm & IPC_SCAP) ? msg->cbuf : NULL;

	switch (cap.sock.mode) {
	case IPC_YIELD:
		*yield_to = recv->pid;
		return YIELD;
	case IPC_NOYIELD:
		proc_release(recv);
		return SYNC;
	default:
		KASSERT(0);
	}
}

err_t cap_sock_reply(proc_t *p, cte_t c, ipc_msg_t *msg, reg_t *yield_to)
{
	cap_t cap = cte_cap(c);

	// check capability type
	err_t err = validate_server_cap(cap, msg, false);
	if (err)
		return err;

	if (proc_is_suspended(p))
		return ERR_SUSPENDED;

	chan_info_t *ci = &chan_infos[cap.sock.chan];

	proc_t *recv = ci->clnt;
	if (!recv || proc_ipc_acquire(recv, cap.sock.chan))
		return ERR_NO_RECEIVER;

	// check capability sending
	send_msg(&recv->tf.a0, ci->clnt_dst, msg, 0);

	switch (cap.sock.mode) {
	case IPC_YIELD:
		proc_release(p);
		*yield_to = recv->pid;
		return YIELD;
	case IPC_NOYIELD:
		proc_release(recv);
		return SUCCESS;
	default:
		KASSERT(0);
	}
}

err_t cap_sock_recv(proc_t *p, cte_t c, ipc_msg_t *msg)
{
	cap_t cap = cte_cap(c);

	// check capability
	err_t err = validate_server_cap(cap, msg, true);
	if (err)
		return err;

	chan_info_t *ci = &chan_infos[cap.sock.chan];

	if (proc_is_suspended(p))
		return ERR_SUSPENDED;

	proc_ipc_wait(p, cap.sock.chan);

	ci->serv = p;
	ci->serv_dst = msg->cbuf;
	return SYNC;
}

err_t cap_sock_replyrecv(proc_t *p, cte_t c, ipc_msg_t *msg, reg_t *yield_to)
{
	cap_t cap = cte_cap(c);

	// check capability type
	err_t err = validate_server_cap(cap, msg, false);
	if (err)
		return err;

	if (proc_is_suspended(p))
		return ERR_SUSPENDED;

	chan_info_t *ci = &chan_infos[cap.sock.chan];

	proc_t *recv = ci->clnt;
	if (!recv || proc_ipc_acquire(recv, cap.sock.chan))
		return ERR_NO_RECEIVER;

	// check capability sending
	send_msg(&recv->tf.a0, ci->clnt_dst, msg, 0);

	proc_ipc_wait(p, cap.sock.chan);

	switch (cap.sock.mode) {
	case IPC_YIELD:
		*yield_to = recv->pid;
		return YIELD;
	case IPC_NOYIELD:
		proc_release(recv);
		return SYNC;
	default:
		KASSERT(0);
	}
}
