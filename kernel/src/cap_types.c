#include "cap_types.h"

#include "pmp.h"

cap_t cap_mk_time(uint64_t hart, uint64_t bgn, uint64_t end)
{
	cap_t cap;
	cap.type = CAPTY_TIME;
	cap.time.hart = hart;
	cap.time.bgn = bgn;
	cap.time.mrk = cap.time.bgn;
	cap.time.end = end;
	return cap;
}

cap_t cap_mk_memory(uint64_t bgn, uint64_t end, uint64_t rwx)
{
	uint64_t offset = bgn >> 15;

	cap_t cap;
	cap.type = CAPTY_MEMORY;
	cap.mem.offset = offset;
	cap.mem.bgn = (bgn - (offset << 15));
	cap.mem.end = (end - (offset << 15));
	cap.mem.mrk = cap.mem.bgn;
	cap.mem.rwx = (rwx & 0x7);
	cap.mem.lck = 0;
	return cap;
}

cap_t cap_mk_pmp(uint64_t addr, uint64_t rwx)
{
	cap_t cap;
	cap.type = CAPTY_PMP;
	cap.pmp.addr = addr & 0xFFFFFFFFFF;
	cap.pmp.rwx = rwx & 0x7;
	cap.pmp.used = 0;
	cap.pmp.slot = 0;
	return cap;
}

cap_t cap_mk_monitor(uint64_t bgn, uint64_t end)
{
	cap_t cap;
	cap.type = CAPTY_MONITOR;
	cap.mon.bgn = bgn & 0xFFFF;
	cap.mon.end = end & 0xFFFF;
	cap.mon.mrk = cap.mon.bgn;
	return cap;
}

cap_t cap_mk_channel(uint64_t bgn, uint64_t end)
{
	cap_t cap;
	cap.type = CAPTY_CHANNEL;
	cap.chan.bgn = bgn & 0xFFFF;
	cap.chan.end = end & 0xFFFF;
	cap.chan.mrk = cap.chan.bgn;
	return cap;
}

cap_t cap_mk_socket(uint64_t chan, uint64_t mode, uint64_t perm, uint64_t tag)
{
	cap_t cap;
	cap.type = CAPTY_SOCKET;
	cap.sock.chan = chan & 0xFFFF;
	cap.sock.mode = mode & 0xF;
	cap.sock.perm = perm & 0xFF;
	cap.sock.tag = tag & 0xFFFFFFFF;
	return cap;
}

static inline bool is_range_subset(uint64_t a_bgn, uint64_t a_end, uint64_t b_bgn, uint64_t b_end)
{
	return a_bgn <= b_bgn && b_end <= a_end;
}

static inline bool is_range_prefix(uint64_t a_bgn, uint64_t a_end, uint64_t b_bgn, uint64_t b_end)
{
	return a_bgn == b_bgn && b_end <= a_end;
}

static inline bool is_bit_subset(uint64_t a, uint64_t b)
{
	return (a & b) == a;
}

static bool cap_time_revokable(cap_t p, cap_t c)
{
	KASSERT(p.type == CAPTY_TIME);
	return (c.type == CAPTY_TIME) && (p.time.hart == c.time.hart)
	       && is_range_subset(p.time.bgn, p.time.end, c.time.bgn, c.time.end);
}

static bool cap_mem_revokable(cap_t p, cap_t c)
{
	KASSERT(p.type == CAPTY_MEMORY);
	if (c.type == CAPTY_PMP) {
		uint64_t p_off = (uint64_t)p.mem.offset << 27;
		uint64_t p_bgn = p_off + ((uint64_t)p.mem.mrk << 12);
		uint64_t p_end = p_off + ((uint64_t)p.mem.end << 12);
		uint64_t c_bgn, c_end;
		pmp_napot_decode(c.pmp.addr, &c_bgn, &c_end);
		return is_range_subset(p_bgn, p_end, c_bgn, c_end);
	}
	return (c.type == CAPTY_MEMORY) && (p.mem.offset == c.mem.offset)
	       && is_range_subset(p.mem.bgn, p.mem.end, c.mem.bgn, c.mem.end);
}

static bool cap_mon_revokable(cap_t p, cap_t c)
{
	KASSERT(p.type == CAPTY_MONITOR);
	return (c.type == CAPTY_MONITOR) && is_range_subset(p.mon.bgn, p.mon.end, c.mon.bgn, c.mon.end);
}

static bool cap_chan_revokable(cap_t p, cap_t c)
{
	KASSERT(p.type == CAPTY_CHANNEL);
	if (c.type == CAPTY_SOCKET) {
		return is_range_subset(p.chan.bgn, p.chan.end, c.sock.chan, c.sock.chan + 1);
	}
	return (c.type == CAPTY_CHANNEL) && is_range_subset(p.chan.bgn, p.chan.end, c.chan.bgn, c.chan.end);
}

static bool cap_sock_revokable(cap_t p, cap_t c)
{
	KASSERT(p.type == CAPTY_SOCKET);
	return (p.sock.tag == 0) && (c.sock.tag != 0) && (p.sock.chan == c.sock.chan);
}

bool cap_revokable(cap_t p, cap_t c)
{
	switch (p.type) {
	case CAPTY_TIME:
		return cap_time_revokable(p, c);
	case CAPTY_MEMORY:
		return cap_mem_revokable(p, c);
	case CAPTY_MONITOR:
		return cap_mon_revokable(p, c);
	case CAPTY_CHANNEL:
		return cap_chan_revokable(p, c);
	case CAPTY_SOCKET:
		return cap_sock_revokable(p, c);
	default:
		return false;
	}
}

static bool cap_valid(cap_t c)
{
	switch (c.type) {
	case CAPTY_TIME:
		return (c.time.bgn == c.time.mrk) && (c.time.bgn < c.time.end);
	case CAPTY_MEMORY:
		return (c.mem.bgn == c.mem.mrk) && (c.mem.bgn < c.mem.end);
	case CAPTY_PMP:
		return (c.pmp.used == 0) && (c.pmp.slot == 0);
	case CAPTY_MONITOR:
		return (c.mon.bgn == c.mon.mrk) && (c.mon.bgn < c.mon.end);
	case CAPTY_CHANNEL:
		return (c.chan.bgn == c.chan.mrk) && (c.chan.bgn < c.chan.end);
	case CAPTY_SOCKET:
		return is_bit_subset(c.sock.perm, IPC_SDATA | IPC_CDATA | IPC_SCAP | IPC_CCAP)
		       && is_bit_subset(c.sock.mode, IPC_YIELD | IPC_NOYIELD);
	default:
		return false;
	}
}

static bool cap_time_derivable(cap_t p, cap_t c)
{
	KASSERT(p.type == CAPTY_TIME);
	return (c.type == CAPTY_TIME) && (p.time.hart == c.time.hart)
	       && is_range_prefix(p.time.bgn, p.time.end, c.time.bgn, c.time.end);
}

static bool cap_mem_derivable(cap_t p, cap_t c)
{
	KASSERT(p.type == CAPTY_MEMORY);
	if (c.type == CAPTY_PMP) {
		uint64_t p_off = (uint64_t)p.mem.offset << 27;
		uint64_t p_mark = p_off + ((uint64_t)p.mem.mrk << 12);
		uint64_t p_end = p_off + ((uint64_t)p.mem.end << 12);
		uint64_t c_bgn, c_end;
		pmp_napot_decode(c.pmp.addr, &c_bgn, &c_end);
		return is_range_subset(p_mark, p_end, c_bgn, c_end) && is_bit_subset(c.pmp.rwx, p.mem.rwx);
	}
	return (c.type == CAPTY_MEMORY) && (p.mem.offset == c.mem.offset)
	       && is_range_subset(p.mem.mrk, p.mem.end, c.mem.bgn, c.mem.end) && is_bit_subset(c.mem.rwx, p.mem.rwx);
}

static bool cap_mon_derivable(cap_t p, cap_t c)
{
	KASSERT(p.type == CAPTY_MONITOR);
	return (c.type == CAPTY_MONITOR) && is_range_subset(p.mon.mrk, p.mon.end, c.mon.bgn, c.mon.end);
}

static bool cap_chan_derivable(cap_t p, cap_t c)
{
	KASSERT(p.type == CAPTY_CHANNEL);
	if (c.type == CAPTY_SOCKET) {
		return (p.sock.tag == 0) && is_range_subset(p.chan.mrk, p.chan.end, c.sock.chan, c.sock.chan + 1);
	}
	return (c.type == CAPTY_CHANNEL) && is_range_subset(p.chan.mrk, p.chan.end, c.chan.bgn, c.chan.end);
}

static bool cap_sock_derivable(cap_t p, cap_t c)
{
	KASSERT(p.type == CAPTY_SOCKET);
	return (c.type == CAPTY_SOCKET) && (p.sock.chan == c.sock.chan) && (p.sock.tag == 0) && (c.sock.tag != 0)
	       && (p.sock.mode == c.sock.mode) && (p.sock.perm == c.sock.perm);
}

bool cap_derivable(cap_t p, cap_t c)
{
	if (!cap_valid(c))
		return false;
	switch (p.type) {
	case CAPTY_TIME:
		return cap_time_derivable(p, c);
	case CAPTY_MEMORY:
		return cap_mem_derivable(p, c);
	case CAPTY_MONITOR:
		return cap_mon_derivable(p, c);
	case CAPTY_CHANNEL:
		return cap_chan_derivable(p, c);
	case CAPTY_SOCKET:
		return cap_sock_derivable(p, c);
	default:
		return false;
	}
}
