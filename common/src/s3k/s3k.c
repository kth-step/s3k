#include "s3k/s3k.h"

typedef union {
	struct {
		uint64_t a0, a1, a2, a3, a4, a5, a6, a7;
	};

	struct {
		int info;
	} get_info;

	struct {
		s3k_reg_t reg;
		uint64_t val;
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
		uint64_t val;
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
		uint64_t data[4];
	} sock;
} sys_args_t;

typedef struct {
	s3k_err_t err;
	uint64_t val;
} s3k_ret_t;

_Static_assert(sizeof(sys_args_t) == 64, "sys_args_t has the wrong size");

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

void s3k_napot_decode(s3k_napot_t addr, uint64_t *base, size_t *size)
{
	*base = ((addr + 1) & addr) << 2;
	*size = (((addr + 1) ^ addr) + 1) << 2;
}

s3k_napot_t s3k_napot_encode(s3k_addr_t base, size_t size)
{
	return (base | (size / 2 - 1)) >> 2;
}

static inline bool is_range_subset(uint64_t a_bgn, uint64_t a_end,
				   uint64_t b_bgn, uint64_t b_end)
{
	return a_bgn <= b_bgn && b_end <= a_end;
}

static inline bool is_range_prefix(uint64_t a_bgn, uint64_t a_end,
				   uint64_t b_bgn, uint64_t b_end)
{
	return a_bgn == b_bgn && b_end <= a_end;
}

static inline bool is_bit_subset(uint64_t a, uint64_t b)
{
	return (a & b) == a;
}

s3k_addr_t s3k_tag_block_to_addr(s3k_tag_t tag, s3k_block_t block)
{
	return ((uint64_t)tag << S3K_MAX_BLOCK_SIZE)
	       + ((uint64_t)block << S3K_MIN_BLOCK_SIZE);
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
		uint64_t p_bgn, p_end, c_bgn, c_end;
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
		uint64_t p_mrk, p_end, c_bgn, c_end;
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

static inline s3k_ret_t do_ecall(s3k_syscall_t call, sys_args_t args)
{
	register uint64_t t0 __asm__("t0") = call;
	register uint64_t a0 __asm__("a0") = args.a0;
	register uint64_t a1 __asm__("a1") = args.a1;
	register uint64_t a2 __asm__("a2") = args.a2;
	register uint64_t a3 __asm__("a3") = args.a3;
	register uint64_t a4 __asm__("a4") = args.a4;
	register uint64_t a5 __asm__("a5") = args.a5;
	register uint64_t a6 __asm__("a6") = args.a6;
	register uint64_t a7 __asm__("a7") = args.a7;
	__asm__ volatile("ecall"
			 : "+r"(t0), "+r"(a0)
			 : "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6),
			   "r"(a7));
	return (s3k_ret_t){.err = t0, .val = a0};
}

uint64_t s3k_get_pid(void)
{
	sys_args_t args = {.get_info = {0}};
	return do_ecall(S3K_SYS_GET_INFO, args).val;
}

uint64_t s3k_get_time(void)
{
	sys_args_t args = {.get_info = {1}};
	return do_ecall(S3K_SYS_GET_INFO, args).val;
}

uint64_t s3k_get_timeout(void)
{
	sys_args_t args = {.get_info = {2}};
	return do_ecall(S3K_SYS_GET_INFO, args).val;
}

uint64_t s3k_get_wcet(bool reset)
{
	sys_args_t args = {.get_info = {reset ? 4 : 3}};
	return do_ecall(S3K_SYS_GET_INFO, args).val;
}

uint64_t s3k_reg_read(s3k_reg_t reg)
{
	sys_args_t args = {.reg = {reg}};
	return do_ecall(S3K_SYS_REG_READ, args).val;
}

uint64_t s3k_reg_write(s3k_reg_t reg, uint64_t val)
{
	sys_args_t args = {
	    .reg = {reg, val}
	     };
	return do_ecall(S3K_SYS_REG_WRITE, args).val;
}

void s3k_sync(void)
{
	sys_args_t args = {.sync = {true}};
	do_ecall(S3K_SYS_SYNC, args);
}

void s3k_sync_mem(void)
{
	sys_args_t args = {.sync = {false}};
	do_ecall(S3K_SYS_SYNC, args);
}

s3k_err_t s3k_cap_read(s3k_cidx_t idx, s3k_cap_t *cap)
{
	sys_args_t args = {.cap = {idx}};
	s3k_ret_t ret = do_ecall(S3K_SYS_CAP_READ, args);
	if (!ret.err)
		cap->raw = ret.val;
	return ret.err;
}

s3k_err_t s3k_cap_move(s3k_cidx_t src, s3k_cidx_t dst)
{
	s3k_err_t err;
	do {
		err = s3k_try_cap_move(src, dst);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_cap_delete(s3k_cidx_t idx)
{
	s3k_err_t err;
	do {
		err = s3k_try_cap_delete(idx);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_cap_revoke(s3k_cidx_t idx)
{
	s3k_err_t err;
	do {
		err = s3k_try_cap_revoke(idx);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_cap_derive(s3k_cidx_t src, s3k_cidx_t dst, s3k_cap_t ncap)
{
	s3k_err_t err;
	do {
		err = s3k_try_cap_derive(src, dst, ncap);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_pmp_load(s3k_cidx_t idx, s3k_pmp_slot_t slot)
{
	s3k_err_t err;
	do {
		err = s3k_try_pmp_load(idx, slot);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_pmp_unload(s3k_cidx_t idx)
{
	s3k_err_t err;
	do {
		err = s3k_try_pmp_unload(idx);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_mon_suspend(s3k_cidx_t mon_idx, s3k_pid_t pid)
{
	s3k_err_t err;
	do {
		err = s3k_try_mon_suspend(mon_idx, pid);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_mon_resume(s3k_cidx_t mon_idx, s3k_pid_t pid)
{
	s3k_err_t err;
	do {
		err = s3k_try_mon_resume(mon_idx, pid);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_mon_state_get(s3k_cidx_t mon_idx, s3k_pid_t pid,
			    s3k_state_t *state)
{
	s3k_err_t err;
	do {
		err = s3k_try_mon_state_get(mon_idx, pid, state);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_mon_yield(s3k_cidx_t mon_idx, s3k_pid_t pid)
{
	s3k_err_t err;
	do {
		err = s3k_try_mon_yield(mon_idx, pid);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_mon_reg_read(s3k_cidx_t mon_idx, s3k_pid_t pid, s3k_reg_t reg,
			   uint64_t *val)
{
	s3k_err_t err;
	do {
		err = s3k_try_mon_reg_read(mon_idx, pid, reg, val);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_mon_reg_write(s3k_cidx_t mon_idx, s3k_pid_t pid, s3k_reg_t reg,
			    uint64_t val)
{
	s3k_err_t err;
	do {
		err = s3k_try_mon_reg_write(mon_idx, pid, reg, val);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_mon_cap_read(s3k_cidx_t mon_idx, s3k_pid_t pid, s3k_cidx_t idx,
			   s3k_cap_t *cap)
{
	s3k_err_t err;
	do {
		err = s3k_try_mon_cap_read(mon_idx, pid, idx, cap);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_mon_cap_move(s3k_cidx_t mon_idx, s3k_pid_t src_pid,
			   s3k_cidx_t src_idx, s3k_pid_t dst_pid,
			   s3k_cidx_t dst_idx)
{
	s3k_err_t err;
	do {
		err = s3k_try_mon_cap_move(mon_idx, src_pid, src_idx, dst_pid,
					   dst_idx);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_mon_pmp_load(s3k_cidx_t mon_idx, s3k_pid_t pid,
			   s3k_cidx_t pmp_idx, s3k_pmp_slot_t pmp_slot)
{
	s3k_err_t err;
	do {
		err = s3k_try_mon_pmp_load(mon_idx, pid, pmp_idx, pmp_slot);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_mon_pmp_unload(s3k_cidx_t mon_idx, s3k_pid_t pid,
			     s3k_cidx_t pmp_idx)
{
	s3k_err_t err;
	do {
		err = s3k_try_mon_pmp_unload(mon_idx, pid, pmp_idx);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_sock_send(s3k_cidx_t sock_idx, const s3k_msg_t *msg)
{
	s3k_err_t err;
	do {
		err = s3k_try_sock_send(sock_idx, msg);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_reply_t s3k_sock_recv(s3k_cidx_t sock_idx, s3k_cidx_t cap_idx)
{
	s3k_reply_t reply;
	do {
		reply = s3k_try_sock_recv(sock_idx, cap_idx);
	} while (reply.err == S3K_ERR_PREEMPTED);
	return reply;
}

s3k_reply_t s3k_sock_sendrecv(s3k_cidx_t sock_idx, const s3k_msg_t *msg)
{
	s3k_reply_t reply;
	do {
		reply = s3k_try_sock_sendrecv(sock_idx, msg);
	} while (reply.err == S3K_ERR_PREEMPTED);
	return reply;
}

s3k_err_t s3k_try_cap_move(s3k_cidx_t src, s3k_cidx_t dst)
{
	sys_args_t args = {
	    .cap = {src, dst}
	     };
	return do_ecall(S3K_SYS_CAP_MOVE, args).err;
}

s3k_err_t s3k_try_cap_delete(s3k_cidx_t idx)
{
	sys_args_t args = {.cap = {idx}};
	return do_ecall(S3K_SYS_CAP_DELETE, args).err;
}

s3k_err_t s3k_try_cap_revoke(s3k_cidx_t idx)
{
	sys_args_t args = {.cap = {idx}};
	return do_ecall(S3K_SYS_CAP_REVOKE, args).err;
}

s3k_err_t s3k_try_cap_derive(s3k_cidx_t src, s3k_cidx_t dst, s3k_cap_t ncap)
{
	sys_args_t args = {
	    .cap = {src, dst, ncap}
	   };
	return do_ecall(S3K_SYS_CAP_DERIVE, args).err;
}

s3k_err_t s3k_try_pmp_load(s3k_cidx_t idx, s3k_pmp_slot_t slot)
{
	sys_args_t args = {
	    .pmp = {idx, slot}
	      };
	return do_ecall(S3K_SYS_PMP_LOAD, args).err;
}

s3k_err_t s3k_try_pmp_unload(s3k_cidx_t idx)
{
	sys_args_t args = {.pmp = {idx}};
	return do_ecall(S3K_SYS_PMP_UNLOAD, args).err;
}

s3k_err_t s3k_try_mon_suspend(s3k_cidx_t mon, s3k_pid_t pid)
{
	sys_args_t args = {
	    .mon_state = {mon, pid}
	   };
	return do_ecall(S3K_SYS_MON_SUSPEND, args).err;
}

s3k_err_t s3k_try_mon_resume(s3k_cidx_t mon, s3k_pid_t pid)
{
	sys_args_t args = {
	    .mon_state = {mon, pid}
	   };
	return do_ecall(S3K_SYS_MON_RESUME, args).err;
}

s3k_err_t s3k_try_mon_state_get(s3k_cidx_t mon, s3k_pid_t pid,
				s3k_state_t *state)
{
	sys_args_t args = {
	    .mon_state = {mon, pid}
	   };
	s3k_ret_t ret = do_ecall(S3K_SYS_MON_STATE_GET, args);
	*state = ret.val;
	return ret.err;
}

s3k_err_t s3k_try_mon_yield(s3k_cidx_t mon, s3k_pid_t pid)
{
	sys_args_t args = {
	    .mon_state = {mon, pid}
	   };
	return do_ecall(S3K_SYS_MON_YIELD, args).err;
}

s3k_err_t s3k_try_mon_reg_read(s3k_cidx_t mon, s3k_pid_t pid, s3k_reg_t reg,
			       uint64_t *val)
{
	sys_args_t args = {
	    .mon_reg = {mon, pid, reg}
	      };
	s3k_ret_t ret = do_ecall(S3K_SYS_MON_REG_READ, args);
	*val = ret.val;
	return ret.err;
}

s3k_err_t s3k_try_mon_reg_write(s3k_cidx_t mon, s3k_pid_t pid, s3k_reg_t reg,
				uint64_t val)
{
	sys_args_t args = {
	    .mon_reg = {mon, pid, reg, val}
	   };
	s3k_ret_t ret = do_ecall(S3K_SYS_MON_REG_WRITE, args);
	return ret.err;
}

s3k_err_t s3k_try_mon_cap_read(s3k_cidx_t mon_idx, s3k_pid_t pid,
			       s3k_cidx_t idx, s3k_cap_t *cap)
{
	sys_args_t args = {
	    .mon_cap = {mon_idx, pid, idx}
	  };
	s3k_ret_t ret = do_ecall(S3K_SYS_MON_CAP_READ, args);
	if (!ret.err)
		cap->raw = ret.val;
	return ret.err;
}

s3k_err_t s3k_try_mon_cap_move(s3k_cidx_t mon_idx, s3k_pid_t src_pid,
			       s3k_cidx_t src_idx, s3k_pid_t dst_pid,
			       s3k_cidx_t dst_idx)
{
	sys_args_t args = {
	    .mon_cap = {mon_idx, src_pid, src_idx, dst_pid, dst_idx}
	    };
	return do_ecall(S3K_SYS_MON_CAP_MOVE, args).err;
}

s3k_err_t s3k_try_mon_pmp_load(s3k_cidx_t mon_idx, s3k_pid_t pid,
			       s3k_cidx_t pmp_idx, s3k_pmp_slot_t pmp_slot)
{
	sys_args_t args = {
	    .mon_pmp = {mon_idx, pid, pmp_idx, pmp_slot}
	};
	return do_ecall(S3K_SYS_MON_PMP_LOAD, args).err;
}

s3k_err_t s3k_try_mon_pmp_unload(s3k_cidx_t mon_idx, s3k_pid_t pid,
				 s3k_cidx_t pmp_idx)
{
	sys_args_t args = {
	    .mon_pmp = {mon_idx, pid, pmp_idx}
	      };
	return do_ecall(S3K_SYS_MON_PMP_UNLOAD, args).err;
}

s3k_err_t s3k_try_sock_send(s3k_cidx_t sock_idx, const s3k_msg_t *msg)
{
	sys_args_t args = {
	    .sock = {.sock_idx = sock_idx,
		     .cap_idx = msg->cap_idx,
		     .send_cap = msg->send_cap,
		     {msg->data[0], msg->data[1], msg->data[2], msg->data[3]}}
	      };
	return do_ecall(S3K_SYS_SOCK_SEND, args).err;
}

s3k_reply_t s3k_try_sock_recv(s3k_cidx_t sock_idx, s3k_cidx_t cap_idx)
{
	sys_args_t args = {
	    .sock = {.sock_idx = sock_idx, .cap_idx = cap_idx}
	      };
	register uint64_t t0 __asm__("t0") = S3K_SYS_SOCK_SENDRECV;
	register uint64_t a0 __asm__("a0") = args.a0;
	register uint64_t a1 __asm__("a1") = args.a1;
	register uint64_t a2 __asm__("a2") = args.a2;
	register uint64_t a3 __asm__("a3") = args.a3;
	register uint64_t a4 __asm__("a4") = args.a4;
	register uint64_t a5 __asm__("a5") = args.a5;
	register uint64_t a6 __asm__("a6") = args.a6;
	register uint64_t a7 __asm__("a7") = args.a7;
	__asm__ volatile("ecall"
			 : "+r"(t0), "+r"(a0), "+r"(a1), "+r"(a2), "+r"(a3),
			   "+r"(a4), "+r"(a5)
			 : "r"(a6), "r"(a7));
	s3k_reply_t reply;
	reply.err = t0;
	reply.tag = a0;
	reply.cap.raw = a1;
	reply.data[0] = a2;
	reply.data[1] = a3;
	reply.data[2] = a4;
	reply.data[3] = a5;
	return reply;
}

s3k_reply_t s3k_try_sock_sendrecv(s3k_cidx_t sock_idx, const s3k_msg_t *msg)
{
	sys_args_t args = {
	    .sock = {.sock_idx = sock_idx,
		     .cap_idx = msg->cap_idx,
		     .send_cap = msg->send_cap,
		     {msg->data[0], msg->data[1], msg->data[2], msg->data[3]}}
	      };
	register uint64_t t0 __asm__("t0") = S3K_SYS_SOCK_SENDRECV;
	register uint64_t a0 __asm__("a0") = args.a0;
	register uint64_t a1 __asm__("a1") = args.a1;
	register uint64_t a2 __asm__("a2") = args.a2;
	register uint64_t a3 __asm__("a3") = args.a3;
	register uint64_t a4 __asm__("a4") = args.a4;
	register uint64_t a5 __asm__("a5") = args.a5;
	register uint64_t a6 __asm__("a6") = args.a6;
	register uint64_t a7 __asm__("a7") = args.a7;
	__asm__ volatile("ecall"
			 : "+r"(t0), "+r"(a0), "+r"(a1), "+r"(a2), "+r"(a3),
			   "+r"(a4), "+r"(a5)
			 : "r"(a6), "r"(a7));
	s3k_reply_t reply;
	reply.err = t0;
	reply.tag = a0;
	reply.cap.raw = a1;
	reply.data[0] = a2;
	reply.data[1] = a3;
	reply.data[2] = a4;
	reply.data[3] = a5;
	return reply;
}
