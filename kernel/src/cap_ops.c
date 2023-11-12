#include "cap_ops.h"

#include "cap_ipc.h"
#include "cap_util.h"
#include "sched.h"

typedef err_t (*ipc_move_handler)(cte_t, cap_t, cte_t);
typedef err_t (*delete_handler)(cte_t, cap_t);
typedef err_t (*revoke_handler)(cte_t, cap_t);
typedef err_t (*derive_handler)(cte_t, cap_t, cte_t, cap_t);

static err_t cap_ipc_move_time(cte_t src, cap_t cap, cte_t dst);
static err_t cap_ipc_move_memory(cte_t src, cap_t cap, cte_t dst);
static err_t cap_ipc_move_pmp(cte_t src, cap_t cap, cte_t dst);
static err_t cap_ipc_move_monitor(cte_t src, cap_t cap, cte_t dst);
static err_t cap_ipc_move_channel(cte_t src, cap_t cap, cte_t dst);
static err_t cap_ipc_move_socket(cte_t src, cap_t cap, cte_t dst);

static err_t cap_delete_time(cte_t src, cap_t cap);
static err_t cap_delete_memory(cte_t src, cap_t cap);
static err_t cap_delete_pmp(cte_t src, cap_t cap);
static err_t cap_delete_monitor(cte_t src, cap_t cap);
static err_t cap_delete_channel(cte_t src, cap_t cap);
static err_t cap_delete_socket(cte_t src, cap_t cap);

static err_t cap_revoke_time(cte_t src, cap_t cap);
static err_t cap_revoke_memory(cte_t src, cap_t cap);
static err_t cap_revoke_pmp(cte_t src, cap_t cap);
static err_t cap_revoke_monitor(cte_t src, cap_t cap);
static err_t cap_revoke_channel(cte_t src, cap_t cap);
static err_t cap_revoke_socket(cte_t src, cap_t cap);

static err_t cap_derive_time(cte_t src, cap_t cap, cte_t dst, cap_t new_cap);
static err_t cap_derive_memory(cte_t src, cap_t cap, cte_t dst, cap_t new_cap);
static err_t cap_derive_pmp(cte_t src, cap_t cap, cte_t dst, cap_t new_cap);
static err_t cap_derive_monitor(cte_t src, cap_t cap, cte_t dst, cap_t new_cap);
static err_t cap_derive_channel(cte_t src, cap_t cap, cte_t dst, cap_t new_cap);
static err_t cap_derive_socket(cte_t src, cap_t cap, cte_t dst, cap_t new_cap);

static const ipc_move_handler ipc_move_handlers[CAPTY_COUNT] = {
    NULL,
    cap_ipc_move_time,
    cap_ipc_move_memory,
    cap_ipc_move_pmp,
    cap_ipc_move_monitor,
    cap_ipc_move_channel,
    cap_ipc_move_socket,
};
static const delete_handler delete_handlers[CAPTY_COUNT] = {
    NULL,
    cap_delete_time,
    cap_delete_memory,
    cap_delete_pmp,
    cap_delete_monitor,
    cap_delete_channel,
    cap_delete_socket,
};
static const revoke_handler revoke_handlers[CAPTY_COUNT] = {
    NULL,
    cap_revoke_time,
    cap_revoke_memory,
    cap_revoke_pmp,
    cap_revoke_monitor,
    cap_revoke_channel,
    cap_revoke_socket,
};
static const derive_handler derive_handlers[CAPTY_COUNT] = {
    NULL,
    cap_derive_time,
    cap_derive_memory,
    cap_derive_pmp,
    cap_derive_monitor,
    cap_derive_channel,
    cap_derive_socket,
};

err_t cap_read(cte_t c, cap_t *cap)
{
	*cap = cte_cap(c);
	return cap->raw ? SUCCESS : ERR_EMPTY;
}

err_t cap_ipc_move(cte_t src, cte_t dst)
{
	if (cte_is_empty(src))
		return ERR_SRC_EMPTY;
	if (!cte_is_empty(dst))
		return ERR_DST_OCCUPIED;
	cap_t cap = cte_cap(src);
	return ipc_move_handlers[cap.type](src, cap, dst);
}

err_t cap_move(cte_t src, cte_t dst)
{
	if (cte_is_empty(src))
		return ERR_SRC_EMPTY;
	if (!cte_is_empty(dst))
		return ERR_DST_OCCUPIED;
	if (cte_pid(src) != cte_pid(dst))
		return cap_ipc_move(src, dst);
	cte_move(src, dst);
	return SUCCESS;
}

err_t cap_delete(cte_t c)
{
	if (cte_is_empty(c))
		return ERR_EMPTY;
	cap_t cap = cte_cap(c);
	return delete_handlers[cap.type](c, cap);
}

err_t cap_revoke(cte_t parent)
{
	cap_t pcap = cte_cap(parent);
	if (pcap.type == CAPTY_NONE)
		return ERR_EMPTY;
	return revoke_handlers[pcap.type](parent, pcap);
}

err_t cap_derive(cte_t src, cte_t dst, cap_t ncap)
{
	if (cte_is_empty(src))
		return ERR_SRC_EMPTY;

	if (!cte_is_empty(dst))
		return ERR_DST_OCCUPIED;

	cap_t scap = cte_cap(src);
	return derive_handlers[scap.type](src, scap, dst, ncap);
}

/********** HANDLERS ***********/

err_t cap_ipc_move_time(cte_t src, cap_t cap, cte_t dst)
{
	sched_update(cte_pid(dst), cap.time.end, cap.time.hart, cap.time.mrk,
		     cap.time.end);
	cte_move(src, dst);
	return SUCCESS;
}

err_t cap_delete_time(cte_t c, cap_t cap)
{
	sched_delete(cap.time.hart, cap.time.mrk, cap.time.end);
	cte_delete(c);
	return SUCCESS;
}

err_t cap_revoke_time(cte_t parent, cap_t pcap)
{
	cte_t child = cte_next(parent);
	cap_t ccap = cte_cap(child);
	if (ccap.type == CAPTY_TIME && pcap.time.hart == ccap.time.hart
	    && pcap.time.bgn <= ccap.time.bgn) {
		// delete the child
		cte_delete(child);

		// Update schedule.
		uint64_t pid = cte_pid(parent);
		uint64_t end = pcap.time.end;
		uint64_t hartid = pcap.time.hart;
		uint64_t from = ccap.time.mrk;
		uint64_t to = pcap.time.mrk;
		sched_update(pid, end, hartid, from, to);

		// Update parent.
		pcap.time.mrk = ccap.time.mrk;
		cte_set_cap(parent, pcap);
		return -1;
	}

	// Update schedule.
	uint64_t pid = cte_pid(parent);
	uint64_t end = pcap.time.end;
	uint64_t hartid = pcap.time.hart;
	uint64_t from = pcap.time.bgn;
	uint64_t to = pcap.time.mrk;
	sched_update(pid, end, hartid, from, to);

	// Update parent.
	pcap.time.mrk = pcap.time.bgn;
	cte_set_cap(parent, pcap);
	return SUCCESS;
}

err_t cap_derive_time(cte_t src, cap_t cap, cte_t dst, cap_t new_cap)
{
	if (new_cap.type != CAPTY_TIME)
		return ERR_INVALID_DERIVATION;
	if (new_cap.time.bgn != new_cap.time.mrk)
		return ERR_INVALID_DERIVATION;
	if (new_cap.time.bgn >= new_cap.time.end)
		return ERR_INVALID_DERIVATION;
	if (new_cap.time.hart != cap.time.hart)
		return ERR_INVALID_DERIVATION;
	if (new_cap.time.bgn != cap.time.mrk)
		return ERR_INVALID_DERIVATION;
	if (new_cap.time.end >= cap.time.end)
		return ERR_INVALID_DERIVATION;
	sched_update(cte_pid(dst), new_cap.time.end, new_cap.time.hart,
		     new_cap.time.bgn, new_cap.time.end);
	cap.time.mrk = cap.time.bgn;
	cte_set_cap(src, cap);
	cte_insert(dst, new_cap, src);
	return SUCCESS;
}

err_t cap_ipc_move_memory(cte_t src, cap_t cap, cte_t dst)
{
	cte_move(src, dst);
	return SUCCESS;
}

err_t cap_delete_memory(cte_t c, cap_t cap)
{
	cte_delete(c);
	return SUCCESS;
}

err_t cap_revoke_memory(cte_t parent, cap_t pcap)
{
	cte_t child = cte_next(parent);
	cap_t ccap = cte_cap(child);
	if (ccap.type == CAPTY_MEMORY && pcap.mem.tag == ccap.mem.tag
	    && pcap.mem.bgn <= ccap.mem.bgn) {
		// delete the child
		cte_delete(child);

		// Update parent.
		pcap.mem.mrk = ccap.mem.mrk;
		pcap.mem.lck = ccap.mem.lck;
		cte_set_cap(parent, pcap);

		return -1;
	}

	uint64_t base, size;
	pmp_napot_decode(ccap.pmp.addr, &base, &size);

	if (ccap.type == CAPTY_PMP
	    && tag_block_to_addr(pcap.mem.tag, pcap.mem.bgn) <= base) {
		// delete the child
		cte_delete(child);

		// Clear PMP config
		if (ccap.pmp.used) {
			proc_pmp_unload(proc_get(cte_pid(child)),
					ccap.pmp.slot);
		}

		return -1;
	}

	pcap.mem.mrk = pcap.mem.bgn;
	pcap.mem.lck = 0;
	cte_set_cap(parent, pcap);

	return SUCCESS;
}

err_t cap_derive_memory(cte_t src, cap_t cap, cte_t dst, cap_t new_cap)
{
	if (new_cap.type == CAPTY_MEMORY) {
		if (cap.mem.tag != new_cap.mem.tag)
			return ERR_INVALID_DERIVATION;
		if (new_cap.mem.bgn != new_cap.mem.mrk)
			return ERR_INVALID_DERIVATION;
		if (new_cap.mem.bgn >= new_cap.mem.end)
			return ERR_INVALID_DERIVATION;
		if (cap.mem.tag != new_cap.mem.tag)
			return ERR_INVALID_DERIVATION;
		if (new_cap.mem.bgn < cap.mem.mrk)
			return ERR_INVALID_DERIVATION;
		if (new_cap.mem.end >= cap.mem.end)
			return ERR_INVALID_DERIVATION;
		if ((new_cap.mem.rwx & cap.mem.rwx) != new_cap.mem.rwx)
			return ERR_INVALID_DERIVATION;
		if (cap.mem.lck)
			return ERR_INVALID_DERIVATION;
		cap.mem.mrk = new_cap.mem.bgn;
		cte_set_cap(src, cap);
		cte_insert(dst, new_cap, src);
		return SUCCESS;
	}
	if (new_cap.type == CAPTY_PMP) {
		uint64_t pmp_begin, pmp_end;
		pmp_napot_decode(new_cap.pmp.addr, &pmp_begin, &pmp_end);
		pmp_end += pmp_begin;
		uint64_t mem_mrk, mem_end;
		mem_mrk = tag_block_to_addr(cap.mem.tag, cap.mem.mrk);
		mem_end = tag_block_to_addr(cap.mem.tag, cap.mem.end);
		if (new_cap.pmp.used && new_cap.pmp.slot)
			return ERR_INVALID_DERIVATION;
		if (pmp_begin < mem_mrk)
			return ERR_INVALID_DERIVATION;
		if (pmp_end >= mem_end)
			return ERR_INVALID_DERIVATION;
		if ((new_cap.pmp.rwx & cap.mem.rwx) != new_cap.pmp.rwx)
			return ERR_INVALID_DERIVATION;
		cap.mem.lck = true;
		cte_set_cap(src, cap);
		cte_insert(dst, new_cap, src);
		return SUCCESS;
	}
	return ERR_INVALID_DERIVATION;
}

err_t cap_ipc_move_pmp(cte_t src, cap_t cap, cte_t dst)
{
	if (cap.pmp.used) {
		proc_t *proc = proc_get(cte_pid(src));
		proc_pmp_unload(proc, cap.pmp.slot);
		cap.pmp.used = 0;
		cap.pmp.slot = 0;
		cte_set_cap(src, cap);
	}
	cte_move(src, dst);
	return SUCCESS;
}

err_t cap_delete_pmp(cte_t c, cap_t cap)
{
	proc_t *proc = proc_get(cte_pid(c));
	if (cap.pmp.used)
		proc_pmp_unload(proc, cap.pmp.slot);
	cte_delete(c);
	return SUCCESS;
}

err_t cap_revoke_pmp(cte_t parent, cap_t pcap)
{
	return SUCCESS;
}

err_t cap_derive_pmp(cte_t src, cap_t cap, cte_t dst, cap_t new_cap)
{
	return ERR_INVALID_DERIVATION;
}

err_t cap_ipc_move_monitor(cte_t src, cap_t cap, cte_t dst)
{
	cte_move(src, dst);
	return SUCCESS;
}

err_t cap_delete_monitor(cte_t c, cap_t cap)
{
	cte_delete(c);
	return SUCCESS;
}

err_t cap_revoke_monitor(cte_t parent, cap_t pcap)
{
	cte_t child = cte_next(parent);
	cap_t ccap = cte_cap(child);
	if (ccap.type == CAPTY_MONITOR && pcap.mon.bgn <= ccap.mon.bgn) {
		// delete the child
		cte_delete(child);

		// Update parent.
		pcap.mon.mrk = ccap.mon.mrk;
		cte_set_cap(parent, pcap);

		return -1;
	}

	pcap.mon.mrk = pcap.mon.mrk;
	cte_set_cap(parent, pcap);

	return SUCCESS;
}

err_t cap_derive_monitor(cte_t src, cap_t cap, cte_t dst, cap_t new_cap)
{
	if (new_cap.type != CAPTY_MONITOR)
		return ERR_INVALID_DERIVATION;
	if (new_cap.mon.bgn != new_cap.mon.mrk)
		return ERR_INVALID_DERIVATION;
	if (new_cap.mon.bgn >= new_cap.mon.end)
		return ERR_INVALID_DERIVATION;
	if (new_cap.mon.bgn < cap.mon.mrk)
		return ERR_INVALID_DERIVATION;
	if (new_cap.mon.end >= cap.mon.end)
		return ERR_INVALID_DERIVATION;
	cap.mon.mrk = cap.mon.bgn;
	cte_set_cap(src, cap);
	cte_insert(dst, new_cap, src);
	return SUCCESS;
}

err_t cap_ipc_move_channel(cte_t src, cap_t cap, cte_t dst)
{
	cte_move(src, dst);
	return SUCCESS;
}

err_t cap_delete_channel(cte_t c, cap_t cap)
{
	cte_delete(c);
	return SUCCESS;
}

err_t cap_revoke_channel(cte_t parent, cap_t pcap)
{
	cte_t child = cte_next(parent);
	cap_t ccap = cte_cap(child);
	if (ccap.type == CAPTY_CHANNEL && pcap.chan.bgn <= ccap.chan.bgn) {
		// delete the child
		cte_delete(child);

		// Update parent.
		pcap.chan.mrk = ccap.chan.mrk;
		cte_set_cap(parent, pcap);

		return -1;
	}

	if (ccap.type == CAPTY_SOCKET && pcap.chan.bgn <= ccap.sock.chan) {
		// delete the child
		cte_delete(child);

		// Clear socket
		cap_sock_clear(ccap, proc_get(cte_pid(child)));

		return -1;
	}

	pcap.chan.mrk = pcap.chan.mrk;
	cte_set_cap(parent, pcap);

	return SUCCESS;
}

err_t cap_derive_channel(cte_t src, cap_t cap, cte_t dst, cap_t new_cap)
{
	if (new_cap.type == CAPTY_CHANNEL) {
		if (new_cap.chan.bgn != new_cap.chan.mrk)
			return ERR_INVALID_DERIVATION;
		if (new_cap.chan.bgn >= new_cap.chan.end)
			return ERR_INVALID_DERIVATION;
		if (new_cap.chan.bgn < cap.chan.mrk)
			return ERR_INVALID_DERIVATION;
		if (new_cap.chan.end >= cap.chan.end)
			return ERR_INVALID_DERIVATION;
		cap.chan.mrk = cap.chan.bgn;
		cte_set_cap(src, cap);
		cte_insert(dst, new_cap, src);
		return SUCCESS;
	}

	if (new_cap.type == CAPTY_SOCKET) {
		if (new_cap.sock.chan < cap.chan.mrk)
			return ERR_INVALID_DERIVATION;
		if (new_cap.sock.chan >= cap.chan.end)
			return ERR_INVALID_DERIVATION;
		cap.chan.mrk = cap.sock.chan;
		cte_set_cap(src, cap);
		cte_insert(dst, new_cap, src);
		return SUCCESS;
	}
	return ERR_INVALID_DERIVATION;
}

err_t cap_ipc_move_socket(cte_t src, cap_t cap, cte_t dst)
{
	proc_t *proc = proc_get(cte_pid(src));
	cap_sock_clear(cap, proc);
	cte_move(src, dst);
	return SUCCESS;
}

err_t cap_delete_socket(cte_t c, cap_t cap)
{
	proc_t *proc = proc_get(cte_pid(c));
	cap_sock_clear(cap, proc);
	cte_delete(c);
	return SUCCESS;
}

err_t cap_revoke_socket(cte_t parent, cap_t pcap)
{
	cte_t child = cte_next(parent);
	cap_t ccap = cte_cap(child);
	if (ccap.type == CAPTY_SOCKET && pcap.sock.tag == 0
	    && pcap.sock.chan == ccap.sock.chan) {
		return ERR_INVALID_CAPABILITY;

		// delete the child
		cte_delete(child);

		// Clear socket
		cap_sock_clear(ccap, proc_get(cte_pid(child)));

		return -1;
	}

	return SUCCESS;
}

err_t cap_derive_socket(cte_t src, cap_t cap, cte_t dst, cap_t new_cap)
{
	if (new_cap.type != CAPTY_SOCKET)
		return ERR_INVALID_DERIVATION;
	if (new_cap.sock.chan != cap.sock.chan)
		return ERR_INVALID_DERIVATION;
	if (new_cap.sock.perm != cap.sock.perm)
		return ERR_INVALID_DERIVATION;
	if (new_cap.sock.mode != cap.sock.mode)
		return ERR_INVALID_DERIVATION;
	if (new_cap.sock.tag == 0)
		return ERR_INVALID_DERIVATION;
	cte_insert(dst, new_cap, src);
	return SUCCESS;
}
