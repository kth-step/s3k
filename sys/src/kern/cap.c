#include "kern/error.h"
#include "kern/cap.h"

#include "kern/sched.h"

static pid_t _pid(cidx_t c);
static cap_data_t _data(cidx_t c);
static void _update(cidx_t c, cap_data_t data);
static void _insert(cidx_t prev, cidx_t c, cap_data_t data);
static void _move(cidx_t src, cidx_t dst);
static void _delete(cidx_t c);
static err_t _memory_delete(cidx_t src);
static bool _memory_revoke(cidx_t c);
static err_t _memory_derive(cidx_t p, cidx_t c, cap_data_t data);
static err_t _pmp_delete(cidx_t c);
static bool _pmp_revoke(cidx_t c);
static err_t _pmp_derive(cidx_t p, cidx_t c, cap_data_t data);
static err_t _time_delete(cidx_t c);
static bool _time_revoke(cidx_t c);
static err_t _time_derive(cidx_t p, cidx_t c, cap_data_t data);
static err_t _monitor_delete(cidx_t c);
static bool _monitor_revoke(cidx_t c);
static err_t _monitor_derive(cidx_t p, cidx_t c, cap_data_t data);
static err_t _channel_delete(cidx_t c);
static bool _channel_revoke(cidx_t c);
static err_t _channel_derive(cidx_t p, cidx_t c, cap_data_t data);
static err_t _socket_delete(cidx_t c);
static bool _socket_revoke(cidx_t c);
static err_t _socket_derive(cidx_t p, cidx_t c, cap_data_t data);
static void _socket_clear(cap_data_t c, proc_t *proc);

extern const cap_data_t init_caps[];
static cap_t ctable[S3K_CAP_CNT * S3K_PROC_CNT];

void cap_init(void)
{
	for (int i = 0; init_caps[i].type; ++i) {
		ctable[i].data = init_caps[i];
		ctable[i].prev = i;
		ctable[i].next = i;
	}
}

err_t cap_read(cidx_t i, cap_data_t *cdata)
{
	if (!ctable[i].data.type)
		return ERR_EMPTY;
	*cdata = ctable[i].data;
	return SUCCESS;

}

err_t cap_move(cidx_t src, cidx_t dst)
{
	if (!ctable[src].data.type)
		return ERR_SRC_EMPTY;
	if (ctable[dst].data.type)
		return ERR_DST_OCCUPIED;
	_move(src, dst);
	return SUCCESS;
}

err_t cap_delete(cidx_t c)
{
	switch (_data(c).type) {
	case CAPTY_NONE:
		return ERR_EMPTY;
	case CAPTY_MEMORY:
		return _memory_revoke(c);
	case CAPTY_PMP:
		return _pmp_revoke(c);
	case CAPTY_TIME:
		return _time_delete(c);
	case CAPTY_MONITOR:
		return _monitor_delete(c);
	case CAPTY_CHANNEL:
		return _channel_delete(c);
	case CAPTY_SOCKET:
		return _socket_delete(c);
	}
	return SUCCESS;
}

err_t cap_revoke(cidx_t c)
{
	bool (*revoker)(cidx_t);
	switch (_data(c).type) {
	case CAPTY_NONE:
		return ERR_EMPTY;
	case CAPTY_TIME:
		revoker = _time_revoke;
		break;
	case CAPTY_MEMORY:
		revoker = _memory_revoke;
		break;
	case CAPTY_PMP:
		revoker = _pmp_revoke;
		break;
	case CAPTY_MONITOR:
		revoker = _monitor_revoke;
		break;
	case CAPTY_CHANNEL:
		revoker = _channel_revoke;
		break;
	case CAPTY_SOCKET:
		revoker = _socket_revoke;
		break;
	}

	while (revoker(c));
	return SUCCESS;
}

err_t cap_derive(cidx_t src, cidx_t dst, cap_data_t new_cap)
{
	switch (ctable[src].data.type) {
	case CAPTY_NONE:
		return ERR_EMPTY;
	case CAPTY_TIME:
		return _time_derive(src, dst, new_cap);
	case CAPTY_MEMORY:
		return _memory_derive(src, dst, new_cap);
	case CAPTY_PMP:
		return _pmp_derive(src, dst, new_cap);
	case CAPTY_MONITOR:
		return _monitor_derive(src, dst, new_cap);
	case CAPTY_CHANNEL:
		return _channel_derive(src, dst, new_cap);
	case CAPTY_SOCKET:
		return _socket_derive(src, dst, new_cap);
	}
}

pid_t _pid(cidx_t c)
{
	return c / S3K_CAP_CNT;
}

cap_data_t _data(cidx_t c)
{
	return ctable[c].data;
}

void _update(cidx_t src, cap_data_t data)
{
	ctable[src].data = data;
}

void _insert(cidx_t prev, cidx_t c, cap_data_t data)
{
	cidx_t next = ctable[prev].next;
	ctable[c].next = next;
	ctable[c].prev = prev;
	ctable[c].data = data;
	ctable[prev].next = c;
	ctable[next].prev = c;
}

void _move(cidx_t src, cidx_t dst)
{
	cidx_t prev = ctable[src].prev;
	cidx_t next = ctable[src].prev;

	ctable[dst].data = ctable[src].data;
	ctable[dst].prev = prev;
	ctable[dst].next = next;
	ctable[prev].next = dst;
	ctable[next].prev = dst;
	ctable[src].data = CAP_NULL;
}

void _delete(cidx_t c)
{
	cidx_t prev = ctable[c].prev;
	cidx_t next = ctable[c].prev;
	ctable[c].data = CAP_NULL;
	ctable[prev].next = next;
	ctable[next].prev = prev;
}


err_t _memory_delete(cidx_t c)
{
	_delete(c);
	return SUCCESS;
}

bool _memory_revoke(cidx_t parent)
{
	cidx_t child = ctable[parent].next;
	cap_data_t pcap = _data(parent);
	cap_data_t ccap = _data(child);
	if (parent == child)
		return true;
	if (ccap.type == CAPTY_MEMORY && pcap.mem.tag == ccap.mem.tag
	    && pcap.mem.bgn <= ccap.mem.bgn) {
		// delete the child
		_delete(child);

		// Update parent.
		pcap.mem.mrk = ccap.mem.mrk;
		pcap.mem.lck = ccap.mem.lck;
		_update(parent, pcap);

		return pcap.mem.mrk == pcap.mem.bgn && !pcap.mem.lck;
	}

	uint64_t base, size;
	pmp_napot_decode(ccap.pmp.addr, &base, &size);

	if (ccap.type == CAPTY_PMP
	    && tag_block_to_addr(pcap.mem.tag, pcap.mem.bgn) <= base) {
		// delete the child
		_delete(child);

		// Clear PMP config
		if (ccap.pmp.used) {
			proc_pmp_unload(proc_get(_pid(child)),
					ccap.pmp.slot);
		}

		return false;
	}

	pcap.mem.mrk = pcap.mem.bgn;
	pcap.mem.lck = 0;
	_update(parent, pcap);

	return true;
}

err_t _memory_derive(cidx_t src, cidx_t dst, cap_data_t new_cap)
{
	cap_data_t cap = _data(src);

	if (new_cap.type == CAPTY_MEMORY && cap.mem.tag == new_cap.mem.tag
	    && cap.mem.tag == new_cap.mem.tag && cap.mem.mrk <= new_cap.mem.bgn
	    && new_cap.mem.end <= cap.mem.end
	    && (new_cap.mem.rwx & cap.mem.rwx) == new_cap.mem.rwx
	    && !cap.mem.lck) {
		cap.mem.mrk = new_cap.mem.end;
		_update(src, cap);
		_insert(src, dst, new_cap);
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
		_update(src, cap);
		_insert(src, dst, new_cap);
		return SUCCESS;
	}
	return ERR_INVDRV;
}

err_t _pmp_delete(cidx_t c)
{
	cap_data_t data = _data(c);
	proc_t *proc = proc_get(_pid(c));
	if (data.pmp.used)
		proc_pmp_unload(proc, data.pmp.slot);
	_delete(c);
	return SUCCESS;
}

bool _pmp_revoke(cidx_t c)
{
	return SUCCESS;
}

err_t _pmp_derive(cidx_t src, cidx_t dst, cap_data_t new_cap)
{
	return ERR_INVDRV;
}

err_t _time_delete(cidx_t c)
{
	cap_data_t data = _data(c);
	sched_delete(data.time.hart, data.time.mrk, data.time.end);
	_delete(c);
	return SUCCESS;
}

bool _time_revoke(cidx_t parent)
{
	cidx_t child = ctable[parent].next;
	cap_data_t pcap = _data(parent);
	cap_data_t ccap = _data(child);
	if (parent == child)
		return true;
	if (ccap.type == CAPTY_TIME && pcap.time.hart == ccap.time.hart
	    && pcap.time.bgn <= ccap.time.bgn
	    && ccap.time.end <= pcap.time.end) {
		// delete the child
		_delete(child);

		// Update schedule.
		uint64_t pid = _pid(parent);
		uint64_t end = pcap.time.end;
		uint64_t hartid = pcap.time.hart;
		uint64_t from = ccap.time.mrk;
		uint64_t to = pcap.time.mrk;
		sched_update(pid, end, hartid, from, to);

		// Update parent.
		pcap.time.mrk = ccap.time.mrk;
		_update(parent, pcap);
		return pcap.time.mrk == pcap.time.bgn;
	}

	// Update schedule.
	uint64_t pid = _pid(parent);
	uint64_t end = pcap.time.end;
	uint64_t hartid = pcap.time.hart;
	uint64_t from = pcap.time.bgn;
	uint64_t to = pcap.time.mrk;
	sched_update(pid, end, hartid, from, to);

	// Update parent.
	pcap.time.mrk = pcap.time.bgn;
	_update(parent, pcap);
	return true;
}

err_t _time_derive(cidx_t src, cidx_t dst, cap_data_t new_cap)
{
	cap_data_t cap = _data(src);
	if (new_cap.type == CAPTY_TIME && new_cap.time.hart == cap.time.hart
	    && new_cap.time.bgn == cap.time.mrk
	    && new_cap.time.end <= cap.time.end) {
		sched_update(_pid(dst), new_cap.time.end, new_cap.time.hart,
			     new_cap.time.bgn, new_cap.time.end);
		cap.time.mrk = new_cap.time.end;
		_update(src, cap);
		_insert(src, dst, new_cap);
		return SUCCESS;
	}
	return ERR_INVDRV;
}

err_t _monitor_delete(cidx_t c)
{
	_delete(c);
	return SUCCESS;
}

bool _monitor_revoke(cidx_t parent)
{
	cidx_t child = ctable[parent].next;
	cap_data_t pcap = _data(parent);
	cap_data_t ccap = _data(child);
	if (parent == child)
		return true;
	if (ccap.type == CAPTY_MONITOR && pcap.mon.bgn <= ccap.mon.bgn) {
		// delete the child
		_delete(child);

		// Update parent.
		pcap.mon.mrk = ccap.mon.mrk;
		_update(parent, pcap);

		return pcap.mon.mrk == pcap.mon.bgn;
	}

	pcap.mon.mrk = pcap.mon.bgn;
	_update(parent, pcap);

	return true;
}

err_t _monitor_derive(cidx_t src, cidx_t dst, cap_data_t new_cap)
{
	cap_data_t cap = _data(src);
	if (new_cap.type == CAPTY_MONITOR && cap.mon.mrk <= new_cap.mon.bgn
	    && new_cap.mon.end <= cap.mon.end) {
		cap.mon.mrk = new_cap.mon.end;
		_update(src, cap);
		_insert(src, dst, new_cap);
		return SUCCESS;
	}
	return ERR_INVDRV;
}

err_t _channel_delete(cidx_t c)
{
	_delete(c);
	return SUCCESS;
}

bool _channel_revoke(cidx_t parent)
{
	cidx_t child = ctable[parent].next;
	cap_data_t pcap = _data(parent);
	cap_data_t ccap = _data(child);
	if (parent == child)
		return true;
	if (ccap.type == CAPTY_CHANNEL && pcap.chan.bgn <= ccap.chan.bgn) {
		// delete the child
		_delete(child);

		// Update parent.
		pcap.chan.mrk = ccap.chan.mrk;
		_update(parent, pcap);

		return pcap.chan.mrk == pcap.chan.bgn;
	}

	if (ccap.type == CAPTY_SOCKET && pcap.chan.bgn <= ccap.sock.chan) {
		// delete the child
		_delete(child);

		// Clear socket
		_socket_clear(ccap, proc_get(_pid(child)));

		return false;
	}

	pcap.chan.mrk = pcap.chan.bgn;
	_update(parent, pcap);

	return true;
}

err_t _channel_derive(cidx_t src, cidx_t dst, cap_data_t new_cap)
{
	cap_data_t cap = _data(src);
	if (new_cap.type == CAPTY_CHANNEL && cap.chan.mrk <= new_cap.chan.bgn
	    && new_cap.chan.end <= cap.chan.end) {
		cap.chan.mrk = new_cap.chan.end;
		_update(src, cap);
		_insert(src, dst, new_cap);
		return SUCCESS;
	}

	if (new_cap.type == CAPTY_SOCKET && cap.chan.mrk <= new_cap.sock.chan
	    && new_cap.sock.chan < cap.chan.end) {
		cap.chan.mrk = new_cap.sock.chan + 1;
		_update(src, cap);
		_insert(src, dst, new_cap);
		return SUCCESS;
	}
	return ERR_INVDRV;
}

err_t _socket_delete(cidx_t c)
{
	proc_t *proc = proc_get(_pid(c));
	cap_data_t data = _data(c);
	_socket_clear(data, proc);
	_delete(c);
	return SUCCESS;
}

bool _socket_revoke(cidx_t parent)
{
	cidx_t child = ctable[parent].next;
	cap_data_t pcap = _data(parent);
	cap_data_t ccap = _data(child);
	if (parent == child)
		return true;
	if (ccap.type == CAPTY_SOCKET && pcap.sock.chan == ccap.sock.chan
	    && pcap.sock.tag == 0) {
		// delete the child
		_delete(child);

		// Clear socket
		_socket_clear(ccap, proc_get(_pid(child)));

		return false;
	}

	return true;
}

err_t _socket_derive(cidx_t src, cidx_t dst, cap_data_t new_cap)
{
	cap_data_t cap = _data(src);
	if (new_cap.type == CAPTY_SOCKET && new_cap.sock.chan == cap.sock.chan
	    && new_cap.sock.perm == cap.sock.perm
	    && new_cap.sock.mode == cap.sock.mode && cap.sock.tag == 0
	    && new_cap.sock.tag != 0) {
		_insert(src, dst, new_cap);
		return SUCCESS;
	}
	return ERR_INVDRV;
}

void _socket_clear(cap_data_t c, proc_t *proc)
{
}
