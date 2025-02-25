#include "cap/ops.h"

#include "cap/ipc.h"
#include "cap/util.h"
#include "kernel.h"
#include "sched.h"

typedef err_t (*ipc_move_handler)(cte_t, cap_t, cte_t);
typedef err_t (*delete_handler)(cte_t, cap_t);
typedef err_t (*revoke_handler)(cte_t, cap_t);
typedef err_t (*derive_handler)(cte_t, cap_t, cte_t, cap_t);

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

static void cap_ipc_move_hook(cte_t src, cte_t dst)
{
	cap_t cap = cte_cap(src);
	switch (cap.type) {
	case CAPTY_TIME: {
		sched_update(cte_pid(dst), cap.time.end, cap.time.hart,
			     cap.time.mrk, cap.time.end);
	} break;
	case CAPTY_PMP: {
		if (cap.pmp.used) {
			proc_pmp_unload(proc_get(cte_pid(src)), cap.pmp.slot);
			cap.pmp.used = 0;
			cap.pmp.slot = 0;
			cte_set_cap(src, cap);
		}
	} break;
	default:
		break;
	}
}

err_t cap_move(cte_t src, cte_t dst)
{
	if (cte_is_empty(src))
		return ERR_SRC_EMPTY;
	if (!cte_is_empty(dst))
		return ERR_DST_OCCUPIED;
	if (cte_pid(src) != cte_pid(dst))
		cap_ipc_move_hook(src, dst);
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
	int err;
	do {
		err = revoke_handlers[pcap.type](parent, cte_cap(parent));
	} while (err < 0 && !kernel_preempt());
	return err < 0 ? ERR_PREEMPTED : SUCCESS;
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
	    && pcap.time.bgn <= ccap.time.bgn
	    && ccap.time.end <= pcap.time.end) {
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
		return pcap.time.mrk == pcap.time.bgn ? SUCCESS : CONTINUE;
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
	if (new_cap.type == CAPTY_TIME && new_cap.time.hart == cap.time.hart
	    && new_cap.time.bgn == cap.time.mrk
	    && new_cap.time.end <= cap.time.end) {
		sched_update(cte_pid(dst), new_cap.time.end, new_cap.time.hart,
			     new_cap.time.bgn, new_cap.time.end);
		cap.time.mrk = new_cap.time.end;
		cte_set_cap(src, cap);
		cte_insert(dst, new_cap, src);
		return SUCCESS;
	}
	return ERR_INVALID_DERIVATION;
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

		return (pcap.mem.mrk == pcap.mem.bgn && !pcap.mem.lck) ?
			   SUCCESS :
			   CONTINUE;
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

		return CONTINUE;
	}

	pcap.mem.mrk = pcap.mem.bgn;
	pcap.mem.lck = 0;
	cte_set_cap(parent, pcap);

	return SUCCESS;
}

err_t cap_derive_memory(cte_t src, cap_t cap, cte_t dst, cap_t new_cap)
{
	if (new_cap.type == CAPTY_MEMORY && cap.mem.tag == new_cap.mem.tag
	    && cap.mem.tag == new_cap.mem.tag && cap.mem.mrk <= new_cap.mem.bgn
	    && new_cap.mem.end <= cap.mem.end
	    && (new_cap.mem.rwx & cap.mem.rwx) == new_cap.mem.rwx
	    && !cap.mem.lck) {
		cap.mem.mrk = new_cap.mem.end;
		cte_set_cap(src, cap);
		cte_insert(dst, new_cap, src);
		return SUCCESS;
	}

	uint64_t pmp_begin, pmp_end;
	uint64_t mem_mrk, mem_end;
	pmp_napot_decode(new_cap.pmp.addr, &pmp_begin, &pmp_end);
	pmp_end += pmp_begin;
	mem_mrk = tag_block_to_addr(cap.mem.tag, cap.mem.mrk);
	mem_end = tag_block_to_addr(cap.mem.tag, cap.mem.end);

	if (new_cap.type == CAPTY_PMP && mem_mrk <= pmp_begin
	    && pmp_end <= mem_end
	    && (new_cap.pmp.rwx & cap.mem.rwx) == new_cap.pmp.rwx) {
		cap.mem.lck = true;
		cte_set_cap(src, cap);
		cte_insert(dst, new_cap, src);
		return SUCCESS;
	}
	return ERR_INVALID_DERIVATION;
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

		return (pcap.mon.mrk == pcap.mon.bgn) ? SUCCESS : CONTINUE;
	}

	pcap.mon.mrk = pcap.mon.bgn;
	cte_set_cap(parent, pcap);

	return SUCCESS;
}

err_t cap_derive_monitor(cte_t src, cap_t cap, cte_t dst, cap_t new_cap)
{
	if (new_cap.type == CAPTY_MONITOR && cap.mon.mrk <= new_cap.mon.bgn
	    && new_cap.mon.end <= cap.mon.end) {
		cap.mon.mrk = new_cap.mon.end;
		cte_set_cap(src, cap);
		cte_insert(dst, new_cap, src);
		return SUCCESS;
	}
	return ERR_INVALID_DERIVATION;
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

		return (pcap.chan.mrk == pcap.chan.bgn) ? SUCCESS : CONTINUE;
	}

	if (ccap.type == CAPTY_SOCKET && pcap.chan.bgn <= ccap.sock.chan) {
		// delete the child
		cte_delete(child);

		// Clear socket
		cap_sock_clear(ccap, proc_get(cte_pid(child)));

		return CONTINUE;
	}

	pcap.chan.mrk = pcap.chan.bgn;
	cte_set_cap(parent, pcap);

	return SUCCESS;
}

err_t cap_derive_channel(cte_t src, cap_t cap, cte_t dst, cap_t new_cap)
{
	if (new_cap.type == CAPTY_CHANNEL && cap.chan.mrk <= new_cap.chan.bgn
	    && new_cap.chan.end <= cap.chan.end) {
		cap.chan.mrk = new_cap.chan.end;
		cte_set_cap(src, cap);
		cte_insert(dst, new_cap, src);
		return SUCCESS;
	}

	if (new_cap.type == CAPTY_SOCKET && cap.chan.mrk <= new_cap.sock.chan
	    && new_cap.sock.chan < cap.chan.end) {
		cap.chan.mrk = new_cap.sock.chan + 1;
		cte_set_cap(src, cap);
		cte_insert(dst, new_cap, src);
		return SUCCESS;
	}
	return ERR_INVALID_DERIVATION;
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
	if (ccap.type == CAPTY_SOCKET && pcap.sock.chan == ccap.sock.chan
	    && pcap.sock.tag == 0) {
		// delete the child
		cte_delete(child);

		// Clear socket
		cap_sock_clear(ccap, proc_get(cte_pid(child)));

		return CONTINUE;
	}

	return SUCCESS;
}

err_t cap_derive_socket(cte_t src, cap_t cap, cte_t dst, cap_t new_cap)
{
	if (new_cap.type == CAPTY_SOCKET && new_cap.sock.chan == cap.sock.chan
	    && new_cap.sock.perm == cap.sock.perm
	    && new_cap.sock.mode == cap.sock.mode && cap.sock.tag == 0
	    && new_cap.sock.tag != 0) {
		cte_insert(dst, new_cap, src);
		return SUCCESS;
	}
	return ERR_INVALID_DERIVATION;
}
