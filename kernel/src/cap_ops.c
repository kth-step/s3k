#include "cap_ops.h"

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
	}
}

err_t cap_delete(cte_t c)
{
	if (!cte_cap(c).type)
		return ERR_EMPTY;
	delete_hook(c, cte_delete(c));
	return SUCCESS;
}

void cap_reclaim(cte_t p, cap_t pcap, cte_t c, cap_t ccap)
{
	if ((cte_prev(c) != p) || cte_cap(c).raw == ccap.raw)
		return;

	cte_delete(c);

	switch (ccap.type) {
	case CAPTY_TIME: {
		pcap.time.mrk = ccap.time.mrk;
		uint64_t pid = cte_pid(p);
		uint64_t end = pcap.time.end;
		uint64_t hartid = pcap.time.hart;
		uint64_t from = pcap.time.mrk;
		uint64_t to = pcap.time.end;
		sched_update(pid, end, hartid, from, to);
	} break;
	case CAPTY_MEMORY:
		pcap.mem.mrk = ccap.mem.mrk;
		pcap.mem.lck = ccap.mem.lck;
		break;
	case CAPTY_PMP:
		if (ccap.pmp.used) {
			proc_pmp_unload(proc_get(cte_pid(c)), ccap.pmp.slot);
		}
		return;
	case CAPTY_MONITOR:
		pcap.mon.mrk = ccap.mon.mrk;
		break;
	case CAPTY_CHANNEL:
		pcap.chan.mrk = ccap.chan.mrk;
		break;
	case CAPTY_SOCKET:
		return;
	default:
		KASSERT(0);
	}

	cte_set_cap(p, pcap);

	return;
}

err_t cap_reset(cte_t c)
{
	if (!cte_cap(c).type)
		return ERR_EMPTY;

	cap_t cap = cte_cap(c);
	switch (cap.type) {
	case CAPTY_TIME:
		cap.time.mrk = cap.time.bgn;
		break;
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
		scap.time.mrk = ncap.time.bgn;
	} break;
	case CAPTY_MEMORY:
		scap.mem.mrk = ncap.mem.bgn;
		break;
	case CAPTY_PMP:
		scap.mem.lck = true;
		break;
	case CAPTY_MONITOR:
		scap.mon.mrk = ncap.mon.bgn;
		break;
	case CAPTY_CHANNEL:
		scap.chan.mrk = ncap.chan.bgn;
		break;
	case CAPTY_SOCKET:
		if (ncap.sock.tag == 0)
			scap.chan.mrk = ncap.sock.chan + 1;
		break;
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
	if (!cap_derivable(scap, ncap))
		return ERR_INVALID_DERIVATION;
	derive(src, scap, dst, ncap);
	return SUCCESS;
}
