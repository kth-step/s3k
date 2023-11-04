#include "cap_ops.h"

#include "cap_ipc.h"
#include "cap_util.h"
#include "pmp.h"
#include "sched.h"

err_t cap_read(cte_t c, cap_t *cap)
{
	*cap = cte_cap(c);
	return cap->raw ? SUCCESS : ERR_EMPTY;
}

static void ipc_move_hook(cte_t src, cte_t dst)
{
	cap_t cap = cte_cap(src);
	switch (cap.type) {
	case CAPTY_TIME: {
		uint64_t pid = cte_pid(dst);
		uint64_t end = cap.time.end;
		uint64_t hartid = cap.time.hart;
		uint64_t from = cap.time.mrk;
		uint64_t to = cap.time.end;
		sched_update(pid, end, hartid, from, to);
	} break;
	case CAPTY_PMP:
		if (cap.pmp.used) {
			proc_pmp_unload(proc_get(cte_pid(src)), cap.pmp.slot);
			cap.pmp.used = 0;
			cap.pmp.slot = 0;
			cte_set_cap(src, cap);
		}
		break;
	case CAPTY_SOCKET:
		cap_sock_clear(cap, proc_get(cte_pid(src)));
		break;
	default:
		break;
	}
}

err_t cap_move(cte_t src, cte_t dst, cap_t *cap)
{
	if (!cte_cap(src).type)
		return ERR_SRC_EMPTY;

	if (cte_cap(dst).type)
		return ERR_DST_OCCUPIED;

	if (cte_pid(src) != cte_pid(dst))
		ipc_move_hook(src, dst);
	cte_move(src, dst, cap);
	return SUCCESS;
}

static void delete_hook(cte_t c, cap_t cap)
{
	// Clean-up resources
	switch (cap.type) {
	case CAPTY_TIME: {
		uint64_t hartid = cap.time.hart;
		uint64_t from = cap.time.mrk;
		uint64_t end = cap.time.end;
		sched_delete(hartid, from, end);
	} break;
	case CAPTY_PMP:
		if (cap.pmp.used)
			proc_pmp_unload(proc_get(cte_pid(c)), cap.pmp.slot);
		break;
	case CAPTY_SOCKET:
		cap_sock_clear(cap, proc_get(cte_pid(c)));
		break;
	default:
		break;
	}
}

err_t cap_delete(cte_t c)
{
	if (!cte_cap(c).type)
		return ERR_EMPTY;
	delete_hook(c, cte_delete(c));
	return SUCCESS;
}

err_t cap_revoke_time(cte_t parent, cap_t pcap, cte_t child, cap_t ccap)
{
	if (ccap.type == CAPTY_TIME) {
		if (pcap.time.hart != ccap.time.hart)
			return YIELD;

		if (pcap.time.end <= ccap.time.bgn)
			return YIELD;

		// delete the child
		cte_delete(child);

		// Update parent.
		pcap.time.mrk = ccap.time.mrk;
		cte_set_cap(parent, pcap);

		// Update schedule.
		uint64_t pid = cte_pid(parent);
		uint64_t end = pcap.time.end;
		uint64_t hartid = pcap.time.hart;
		uint64_t from = pcap.time.mrk;
		uint64_t to = pcap.time.end;
		sched_update(pid, end, hartid, from, to);

		return SUCCESS;
	}
	return YIELD;
}

err_t cap_revoke_memory(cte_t parent, cap_t pcap, cte_t child, cap_t ccap)
{
	if (ccap.type == CAPTY_MEMORY) {
		if (pcap.mem.tag != ccap.mem.tag)
			return YIELD;

		if (pcap.mem.end <= ccap.mem.bgn)
			return YIELD;

		// delete the child
		cte_delete(child);

		// Update parent.
		pcap.mem.mrk = ccap.mem.mrk;
		pcap.mem.lck = ccap.mem.lck;
		cte_set_cap(parent, pcap);

		return SUCCESS;
	} else if (ccap.type == CAPTY_PMP) {
		uint64_t begin, end;
		pmp_napot_decode(ccap.pmp.addr, &begin, &end);
		if (pcap.mem.end < begin)
			return YIELD;
		// delete the child
		cte_delete(child);

		// Clear PMP config
		if (ccap.pmp.used) {
			proc_pmp_unload(proc_get(cte_pid(child)),
					ccap.pmp.slot);
		}

		return SUCCESS;
	}
	return YIELD;
}

err_t cap_revoke_monitor(cte_t parent, cap_t pcap, cte_t child, cap_t ccap)
{
	if (ccap.type == CAPTY_MONITOR) {
		if (pcap.mon.end <= ccap.mon.bgn)
			return YIELD;

		// delete the child
		cte_delete(child);

		// Update parent.
		pcap.mon.mrk = ccap.mon.mrk;
		cte_set_cap(parent, pcap);

		return SUCCESS;
	}
	return YIELD;
}

err_t cap_revoke_channel(cte_t parent, cap_t pcap, cte_t child, cap_t ccap)
{
	if (ccap.type == CAPTY_CHANNEL) {
		if (pcap.chan.end <= ccap.chan.bgn)
			return YIELD;

		// delete the child
		cte_delete(child);

		// Update parent.
		pcap.chan.mrk = ccap.chan.mrk;
		cte_set_cap(parent, pcap);

		return SUCCESS;
	}

	if (ccap.type == CAPTY_SOCKET) {
		if (pcap.chan.end <= ccap.sock.chan)
			return YIELD;

		// delete the child
		cte_delete(child);

		// Clear socket
		cap_sock_clear(ccap, proc_get(cte_pid(child)));

		return SUCCESS;
	}

	return YIELD;
}

err_t cap_revoke_socket(cte_t parent, cap_t pcap, cte_t child, cap_t ccap)
{
	if (ccap.type == CAPTY_SOCKET) {
		if (pcap.sock.tag != 0)
			return ERR_INVALID_CAPABILITY;
		if (pcap.sock.chan != ccap.sock.chan)
			return YIELD;

		// delete the child
		cte_delete(child);

		// Clear socket
		cap_sock_clear(ccap, proc_get(cte_pid(child)));

		return SUCCESS;
	}

	return YIELD;
}

err_t cap_revoke(cte_t parent)
{
	cte_t child = cte_next(parent);
	cap_t pcap = cte_cap(parent);
	cap_t ccap = cte_cap(child);
	switch (pcap.type) {
	case CAPTY_NONE:
		return ERR_EMPTY;
	case CAPTY_TIME:
		return cap_revoke_time(parent, pcap, child, ccap);
	case CAPTY_MEMORY:
		return cap_revoke_memory(parent, pcap, child, ccap);
	case CAPTY_PMP:
		return ERR_INVALID_CAPABILITY;
	case CAPTY_MONITOR:
		return cap_revoke_monitor(parent, pcap, child, ccap);
	case CAPTY_CHANNEL:
		return cap_revoke_channel(parent, pcap, child, ccap);
	case CAPTY_SOCKET:
		return cap_revoke_socket(parent, pcap, child, ccap);
	default:
		KASSERT(0);
	}
}

err_t cap_reset(cte_t c)
{
	if (!cte_cap(c).type)
		return ERR_EMPTY;

	cap_t cap = cte_cap(c);
	switch (cap.type) {
	case CAPTY_TIME: {
		uint64_t pid = cte_pid(c);
		uint64_t end = cap.time.end;
		uint64_t hartid = cap.time.hart;
		uint64_t from = cap.time.bgn;
		uint64_t to = cap.time.mrk;
		sched_update(pid, end, hartid, from, to);
		cap.time.mrk = cap.time.bgn;
	} break;
	case CAPTY_MEMORY:
		cap.mem.mrk = cap.mem.bgn;
		cap.mem.lck = false;
		break;
	case CAPTY_MONITOR:
		cap.mon.mrk = cap.mon.bgn;
		break;
	case CAPTY_CHANNEL:
		cap.chan.mrk = cap.chan.bgn;
		break;
	default:
		return SUCCESS;
	}

	cte_set_cap(c, cap);

	return SUCCESS;
}

static void derive(cte_t src, cap_t scap, cte_t dst, cap_t ncap)
{
	// Update the original capability
	switch (ncap.type) {
	case CAPTY_TIME: {
		uint64_t pid = cte_pid(dst);
		uint64_t end = ncap.time.end;
		uint64_t hartid = ncap.time.hart;
		uint64_t from = ncap.time.mrk;
		uint64_t to = ncap.time.end;
		sched_update(pid, end, hartid, from, to);
		scap.time.mrk = ncap.time.end;
	} break;
	case CAPTY_MEMORY:
		scap.mem.mrk = ncap.mem.end;
		break;
	case CAPTY_PMP:
		scap.mem.lck = true;
		break;
	case CAPTY_MONITOR:
		scap.mon.mrk = ncap.mon.end;
		break;
	case CAPTY_CHANNEL:
		scap.chan.mrk = ncap.chan.end;
		break;
	case CAPTY_SOCKET:
		if (ncap.sock.tag == 0)
			scap.chan.mrk = ncap.sock.chan + 1;
		break;
	case CAPTY_NONE:
		KASSERT(0);
	}
	cte_insert(dst, ncap, src);
	cte_set_cap(src, scap);
}

err_t cap_derive(cte_t src, cte_t dst, cap_t ncap)
{
	if (!cte_cap(src).type)
		return ERR_SRC_EMPTY;

	if (cte_cap(dst).type)
		return ERR_DST_OCCUPIED;

	cap_t scap = cte_cap(src);
	if (!cap_is_derivable(scap, ncap))
		return ERR_INVALID_DERIVATION;
	derive(src, scap, dst, ncap);
	return SUCCESS;
}
