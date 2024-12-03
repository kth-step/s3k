#include "s3k/s3k.h"

typedef union {
	struct {
		s3k_val_t a0, a1, a2, a3, a4, a5, a6, a7;
	};

	struct {
		int info;
	} get_info;

	struct {
		s3k_reg_t reg;
		s3k_val_t val;
	} reg;

	struct {
		bool full;
	} sync;

	struct {
		s3k_cidx_t idx;
		s3k_cidx_t dst_idx;
		s3k_cap_t cap;
	} cap;

	struct {
		s3k_cidx_t pmp_idx;
		s3k_pmp_slot_t pmp_slot;
	} pmp;

	struct {
		s3k_cidx_t mon_idx;
		s3k_pid_t pid;
	} mon_state;

	struct {
		s3k_cidx_t mon_idx;
		s3k_pid_t pid;
		s3k_reg_t reg;
		s3k_val_t val;
	} mon_reg;

	struct {
		s3k_cidx_t mon_idx;
		s3k_pid_t src_pid;
		s3k_cidx_t src_idx;
		s3k_pid_t dst_pid;
		s3k_cidx_t dst_idx;
	} mon_cap;

	struct {
		s3k_cidx_t mon_idx;
		s3k_pid_t pid;
		s3k_cidx_t pmp_idx;
		s3k_pmp_slot_t pmp_slot;
	} mon_pmp;

	struct {
		s3k_cidx_t sock_idx;
		s3k_cidx_t cap_idx;
		bool send_cap;
		uint8_t data[16];
	} sock;
} sys_args_t;

typedef struct {
	s3k_err_t err;
	s3k_val_t val;
} s3k_ret_t;

_Static_assert(sizeof(sys_args_t) == 32, "sys_args_t has the wrong size");

s3k_cap_t s3k_mk_time(s3k_hart_t hart, s3k_time_slot_t bgn, s3k_time_slot_t end)
{
	return (s3k_cap_t){
	    .time = {
		     .type = S3K_CAPTY_TIME,
		     .hart = hart,
		     .bgn = bgn,
		     .mrk = bgn,
		     .end = end,
		     }
	     };
}

s3k_cap_t s3k_mk_memory(s3k_addr_t bgn, s3k_addr_t end, s3k_rwx_t rwx)
{
	s3k_tag_t tag = bgn >> S3K_MAX_BLOCK_SIZE;
	s3k_block_t bgn_block = (bgn - (tag << S3K_MAX_BLOCK_SIZE))
				>> S3K_MIN_BLOCK_SIZE;
	s3k_block_t end_block = (end - (tag << S3K_MAX_BLOCK_SIZE))
				>> S3K_MIN_BLOCK_SIZE;

	return (s3k_cap_t){
	    .mem = {
		    .type = S3K_CAPTY_MEMORY,
		    .tag = tag,
		    .bgn = bgn_block,
		    .end = end_block,
		    .mrk = bgn_block,
		    .rwx = rwx,
		    .lck = 0,
		    }
	     };
}

s3k_cap_t s3k_mk_pmp(s3k_napot_t addr, s3k_rwx_t rwx)
{
	return (s3k_cap_t){
	    .pmp = {
		    .type = S3K_CAPTY_PMP,
		    .addr = addr & 0xFFFFFFFFFF,
		    .rwx = rwx & 0x7,
		    .used = 0,
		    .slot = 0,
		    }
	     };
}

s3k_cap_t s3k_mk_monitor(s3k_pid_t bgn, s3k_pid_t end)
{
	return (s3k_cap_t){
	    .mon = {
		    .type = S3K_CAPTY_MONITOR,
		    .bgn = bgn,
		    .end = end,
		    .mrk = bgn,
		    }
	     };
}

s3k_cap_t s3k_mk_channel(s3k_chan_t bgn, s3k_chan_t end)
{
	return (s3k_cap_t){
	    .chan = {
		     .type = S3K_CAPTY_CHANNEL,
		     .bgn = bgn,
		     .end = end,
		     .mrk = bgn,
		     }
	     };
}

s3k_cap_t s3k_mk_socket(s3k_chan_t chan, s3k_ipc_mode_t mode,
			s3k_ipc_perm_t perm, uint32_t tag)
{
	return (s3k_cap_t){
	    .sock = {
		     .type = S3K_CAPTY_SOCKET,
		     .chan = chan,
		     .mode = mode,
		     .perm = perm,
		     .tag = tag,
		     }
	     };
}

void s3k_napot_decode(s3k_napot_t addr, s3k_addr_t *base, s3k_addr_t *size)
{
	*base = ((addr + 1) & addr) << 2;
	*size = (((addr + 1) ^ addr) + 1) << 2;
}

s3k_napot_t s3k_napot_encode(s3k_addr_t base, s3k_addr_t size)
{
	return (base | (size / 2 - 1)) >> 2;
}

static inline bool is_range_subset(s3k_addr_t a_bgn, s3k_addr_t a_end,
				   s3k_addr_t b_bgn, s3k_addr_t b_end)
{
	return a_bgn <= b_bgn && b_end <= a_end;
}

static inline bool is_range_prefix(s3k_addr_t a_bgn, s3k_addr_t a_end,
				   s3k_addr_t b_bgn, s3k_addr_t b_end)
{
	return a_bgn == b_bgn && b_end <= a_end;
}

static inline bool is_bit_subset(s3k_addr_t a, s3k_addr_t b)
{
	return (a & b) == a;
}

s3k_addr_t s3k_tag_block_to_addr(s3k_tag_t tag, s3k_block_t block)
{
	return ((s3k_addr_t)tag << S3K_MAX_BLOCK_SIZE)
	       + ((s3k_addr_t)block << S3K_MIN_BLOCK_SIZE);
}

static bool s3k_cap_time_revokable(s3k_cap_t p, s3k_cap_t c)
{
	return (c.type == S3K_CAPTY_TIME) && (p.time.hart == c.time.hart)
	       && is_range_subset(p.time.bgn, p.time.end, c.time.bgn,
				  c.time.end);
}

static bool s3k_cap_mem_revokable(s3k_cap_t p, s3k_cap_t c)
{
	if (c.type == S3K_CAPTY_PMP) {
		s3k_addr_t p_bgn, p_end, c_bgn, c_end;
		p_bgn = s3k_tag_block_to_addr(p.mem.tag, p.mem.bgn);
		p_end = s3k_tag_block_to_addr(p.mem.tag, p.mem.end);
		s3k_napot_decode(c.pmp.addr, &c_bgn, &c_end);
		return is_range_subset(p_bgn, p_end, c_bgn, c_end);
	}
	return (c.type == S3K_CAPTY_MEMORY) && (p.mem.tag == c.mem.tag)
	       && is_range_subset(p.mem.bgn, p.mem.end, c.mem.bgn, c.mem.end);
}

static bool s3k_cap_mon_revokable(s3k_cap_t p, s3k_cap_t c)
{
	return (c.type == S3K_CAPTY_MONITOR)
	       && is_range_subset(p.mon.bgn, p.mon.end, c.mon.bgn, c.mon.end);
}

static bool s3k_cap_chan_revokable(s3k_cap_t p, s3k_cap_t c)
{
	if (c.type == S3K_CAPTY_SOCKET) {
		return is_range_subset(p.chan.bgn, p.chan.end, c.sock.chan,
				       c.sock.chan + 1);
	}
	return (c.type == S3K_CAPTY_CHANNEL)
	       && is_range_subset(p.chan.bgn, p.chan.end, c.chan.bgn,
				  c.chan.end);
}

static bool s3k_cap_sock_revokable(s3k_cap_t p, s3k_cap_t c)
{
	return (p.sock.tag == 0) && (c.sock.tag != 0)
	       && (p.sock.chan == c.sock.chan);
}

bool s3k_cap_is_revokable(s3k_cap_t p, s3k_cap_t c)
{
	switch (p.type) {
	case S3K_CAPTY_TIME:
		return s3k_cap_time_revokable(p, c);
	case S3K_CAPTY_MEMORY:
		return s3k_cap_mem_revokable(p, c);
	case S3K_CAPTY_MONITOR:
		return s3k_cap_mon_revokable(p, c);
	case S3K_CAPTY_CHANNEL:
		return s3k_cap_chan_revokable(p, c);
	case S3K_CAPTY_SOCKET:
		return s3k_cap_sock_revokable(p, c);
	default:
		return false;
	}
}

bool s3k_cap_is_valid(s3k_cap_t c)
{
	switch (c.type) {
	case S3K_CAPTY_TIME:
		return (c.time.bgn == c.time.mrk) && (c.time.bgn < c.time.end);
	case S3K_CAPTY_MEMORY:
		return (c.mem.bgn == c.mem.mrk) && (c.mem.bgn < c.mem.end);
	case S3K_CAPTY_PMP:
		return (c.pmp.used == 0) && (c.pmp.slot == 0);
	case S3K_CAPTY_MONITOR:
		return (c.mon.bgn == c.mon.mrk) && (c.mon.bgn < c.mon.end);
	case S3K_CAPTY_CHANNEL:
		return (c.chan.bgn == c.chan.mrk) && (c.chan.bgn < c.chan.end);
	case S3K_CAPTY_SOCKET:
		return is_bit_subset(c.sock.perm, S3K_IPC_SDATA | S3K_IPC_CDATA
						      | S3K_IPC_SCAP
						      | S3K_IPC_CCAP)
		       && is_bit_subset(c.sock.mode,
					S3K_IPC_YIELD | S3K_IPC_NOYIELD);
	default:
		return false;
	}
}

static bool s3k_cap_time_derivable(s3k_cap_t p, s3k_cap_t c)
{
	return (c.type == S3K_CAPTY_TIME) && (p.time.hart == c.time.hart)
	       && is_range_prefix(p.time.bgn, p.time.end, c.time.bgn,
				  c.time.end);
}

static bool s3k_cap_mem_derivable(s3k_cap_t p, s3k_cap_t c)
{
	if (c.type == S3K_CAPTY_PMP) {
		s3k_addr_t p_mrk, p_end, c_bgn, c_end;
		p_mrk = s3k_tag_block_to_addr(p.mem.tag, p.mem.mrk);
		p_end = s3k_tag_block_to_addr(p.mem.tag, p.mem.end);
		s3k_napot_decode(c.pmp.addr, &c_bgn, &c_end);
		return is_range_subset(p_mrk, p_end, c_bgn, c_end)
		       && is_bit_subset(c.pmp.rwx, p.mem.rwx);
	}
	return (c.type == S3K_CAPTY_MEMORY) && (p.mem.tag == c.mem.tag)
	       && is_range_subset(p.mem.mrk, p.mem.end, c.mem.bgn, c.mem.end)
	       && is_bit_subset(c.mem.rwx, p.mem.rwx);
}

static bool s3k_cap_mon_derivable(s3k_cap_t p, s3k_cap_t c)
{
	return (c.type == S3K_CAPTY_MONITOR)
	       && is_range_subset(p.mon.mrk, p.mon.end, c.mon.bgn, c.mon.end);
}

static bool s3k_cap_chan_derivable(s3k_cap_t p, s3k_cap_t c)
{
	if (c.type == S3K_CAPTY_SOCKET) {
		return (c.sock.tag == 0)
		       && is_range_subset(p.chan.mrk, p.chan.end, c.sock.chan,
					  c.sock.chan + 1);
	}
	return (c.type == S3K_CAPTY_CHANNEL)
	       && is_range_subset(p.chan.mrk, p.chan.end, c.chan.bgn,
				  c.chan.end);
}

static bool s3k_cap_sock_derivable(s3k_cap_t p, s3k_cap_t c)
{
	return (c.type == S3K_CAPTY_SOCKET) && (p.sock.chan == c.sock.chan)
	       && (p.sock.tag == 0) && (c.sock.tag != 0)
	       && (p.sock.mode == c.sock.mode) && (p.sock.perm == c.sock.perm);
}

bool s3k_cap_is_derivable(s3k_cap_t p, s3k_cap_t c)
{
	switch (p.type) {
	case S3K_CAPTY_TIME:
		return s3k_cap_time_derivable(p, c);
	case S3K_CAPTY_MEMORY:
		return s3k_cap_mem_derivable(p, c);
	case S3K_CAPTY_MONITOR:
		return s3k_cap_mon_derivable(p, c);
	case S3K_CAPTY_CHANNEL:
		return s3k_cap_chan_derivable(p, c);
	case S3K_CAPTY_SOCKET:
		return s3k_cap_sock_derivable(p, c);
	default:
		return false;
	}
}
