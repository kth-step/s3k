#include "s3k/s3k.h"

s3k_cap_t s3k_mk_time(uint64_t hart, uint64_t bgn, uint64_t end)
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

s3k_cap_t s3k_mk_memory(uint64_t bgn, uint64_t end, uint64_t rwx)
{
	uint64_t tag = bgn >> S3K_MAX_BLOCK_SIZE;
	bgn = (bgn - (tag << S3K_MAX_BLOCK_SIZE)) >> S3K_MIN_BLOCK_SIZE;
	end = (end - (tag << S3K_MAX_BLOCK_SIZE)) >> S3K_MIN_BLOCK_SIZE;

	return (s3k_cap_t){
	    .mem = {
		    .type = S3K_CAPTY_MEMORY,
		    .tag = tag,
		    .bgn = bgn,
		    .end = end,
		    .mrk = bgn,
		    .rwx = rwx,
		    .lck = 0,
		    }
	     };
}

s3k_cap_t s3k_mk_pmp(uint64_t addr, uint64_t rwx)
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

s3k_cap_t s3k_mk_monitor(uint64_t bgn, uint64_t end)
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

s3k_cap_t s3k_mk_channel(uint64_t bgn, uint64_t end)
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

s3k_cap_t s3k_mk_socket(uint64_t chan, uint64_t mode, uint64_t perm,
			uint64_t tag)
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

void s3k_napot_decode(uint64_t addr, uint64_t *base, uint64_t *size)
{
	*base = ((addr + 1) & addr) << 2;
	*size = (((addr + 1) ^ addr) + 1) << 2;
}

uint64_t s3k_napot_encode(uint64_t base, uint64_t size)
{
	return (base | (size / 2 - 1)) >> 2;
}

uint64_t s3k_get_pid(void)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_GET_INFO;
	register uint64_t a0 __asm__("a0") = 0;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(t0));
	return a0;
}

uint64_t s3k_get_time(void)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_GET_INFO;
	register uint64_t a0 __asm__("a0") = 1;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(t0));
	return a0;
}

uint64_t s3k_get_timeout(void)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_GET_INFO;
	register uint64_t a0 __asm__("a0") = 2;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(t0));
	return a0;
}

uint64_t s3k_read_reg(uint64_t reg)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_REG_READ;
	register uint64_t a0 __asm__("a0") = reg;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(t0));
	return a0;
}

void s3k_write_reg(uint64_t reg, uint64_t val)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_REG_WRITE;
	register uint64_t a0 __asm__("a0") = reg;
	register uint64_t a1 __asm__("a1") = val;
	__asm__ volatile("ecall" ::"r"(a0), "r"(a1), "r"(t0));
}

void s3k_sync(void)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_SYNC;
	__asm__ volatile("ecall" ::"r"(t0));
}

void s3k_sync_mem(void)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_SYNC_MEM;
	__asm__ volatile("ecall" ::"r"(t0));
}

s3k_err_t s3k_cap_read(uint64_t read_idx, s3k_cap_t *cap)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_CAP_READ;
	register uint64_t a0 __asm__("a0") = read_idx;
	__asm__ volatile("ecall" : "+r"(t0), "+r"(a0));
	if (!a0)
		cap->raw = a0;
	return t0;
}

s3k_err_t s3k_cap_move(uint64_t src, uint64_t dst)
{
	s3k_err_t err;
	do {
		err = s3k_try_cap_move(src, dst);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_cap_delete(uint64_t idx)
{
	s3k_err_t err;
	do {
		err = s3k_try_cap_delete(idx);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_cap_revoke(uint64_t idx)
{
	s3k_err_t err;
	do {
		err = s3k_try_cap_revoke(idx);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_cap_derive(uint64_t src, uint64_t dst, s3k_cap_t ncap)
{
	s3k_err_t err;
	do {
		err = s3k_try_cap_derive(src, dst, ncap);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_pmp_load(uint64_t idx, uint64_t slot)
{
	s3k_err_t err;
	do {
		err = s3k_try_pmp_load(idx, slot);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_pmp_unload(uint64_t idx)
{
	s3k_err_t err;
	do {
		err = s3k_try_pmp_unload(idx);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_mon_suspend(uint64_t mon, uint64_t pid)
{
	s3k_err_t err;
	do {
		err = s3k_try_mon_suspend(mon, pid);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_mon_resume(uint64_t mon, uint64_t pid)
{
	s3k_err_t err;
	do {
		err = s3k_try_mon_resume(mon, pid);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_mon_reg_read(uint64_t mon, uint64_t pid, uint64_t reg,
			   uint64_t *val)
{
	s3k_err_t err;
	do {
		err = s3k_try_mon_reg_read(mon, pid, reg, val);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_mon_reg_write(uint64_t mon, uint64_t pid, uint64_t reg,
			    uint64_t val)
{
	s3k_err_t err;
	do {
		err = s3k_try_mon_reg_write(mon, pid, reg, val);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_mon_cap_read(uint64_t mon, uint64_t pid, uint64_t idx,
			   s3k_cap_t *cap)
{
	s3k_err_t err;
	do {
		err = s3k_try_mon_cap_read(mon, pid, idx, cap);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_mon_cap_move(uint64_t mon, uint64_t src_pid, uint64_t src_idx,
			   uint64_t dst_pid, uint64_t dst_idx)
{
	s3k_err_t err;
	do {
		err = s3k_try_mon_cap_move(mon, src_pid, src_idx, dst_pid,
					   dst_idx);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_mon_pmp_load(uint64_t mon, uint64_t pid, uint64_t idx,
			   uint64_t slot)
{
	s3k_err_t err;
	do {
		err = s3k_try_mon_pmp_load(mon, pid, idx, slot);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_mon_pmp_unload(uint64_t mon, uint64_t pid, uint64_t idx)
{
	s3k_err_t err;
	do {
		err = s3k_try_mon_pmp_unload(mon, pid, idx);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_sock_send(uint64_t sock_idx, uint64_t buf_idx, uint64_t data[4],
			uint64_t send_cap)
{
	s3k_err_t err;
	do {
		err = s3k_try_sock_send(sock_idx, buf_idx, data, send_cap);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_sock_sendrecv(uint64_t sock_idx, uint64_t buf_idx,
			    uint64_t data[4], uint64_t send_cap,
			    uint64_t service_time)
{
	s3k_err_t err;
	do {
		err = s3k_try_sock_sendrecv(sock_idx, buf_idx, data, send_cap,
					    service_time);
	} while (err == S3K_ERR_PREEMPTED);
	return err;
}

s3k_err_t s3k_try_cap_move(uint64_t src, uint64_t dst)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_CAP_MOVE;
	register uint64_t a0 __asm__("a0") = src;
	register uint64_t a1 __asm__("a1") = dst;
	__asm__ volatile("ecall" : "+r"(t0), "+r"(a0) : "r"(a1));
	return t0;
}

s3k_err_t s3k_try_cap_delete(uint64_t idx)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_CAP_DELETE;
	register uint64_t a0 __asm__("a0") = idx;
	__asm__ volatile("ecall" : "+r"(t0), "+r"(a0));
	return t0;
}

s3k_err_t s3k_try_cap_revoke(uint64_t idx)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_CAP_REVOKE;
	register uint64_t a0 __asm__("a0") = idx;
	__asm__ volatile("ecall" : "+r"(t0), "+r"(a0));
	return t0;
}

s3k_err_t s3k_try_cap_derive(uint64_t src, uint64_t dst, s3k_cap_t ncap)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_CAP_DERIVE;
	register uint64_t a0 __asm__("a0") = src;
	register uint64_t a1 __asm__("a1") = dst;
	register uint64_t a2 __asm__("a2") = ncap.raw;
	__asm__ volatile("ecall" : "+r"(t0), "+r"(a0) : "r"(a1), "r"(a2));
	return t0;
}

s3k_err_t s3k_try_pmp_load(uint64_t idx, uint64_t slot)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_PMP_LOAD;
	register uint64_t a0 __asm__("a0") = idx;
	register uint64_t a1 __asm__("a1") = slot;
	__asm__ volatile("ecall" : "+r"(t0), "+r"(a0) : "r"(a1));
	return t0;
}

s3k_err_t s3k_try_pmp_unload(uint64_t idx)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_PMP_UNLOAD;
	register uint64_t a0 __asm__("a0") = idx;
	__asm__ volatile("ecall" : "+r"(t0), "+r"(a0));
	return t0;
}

s3k_err_t s3k_try_mon_suspend(uint64_t mon, uint64_t pid)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_MON_SUSPEND;
	register uint64_t a0 __asm__("a0") = mon;
	register uint64_t a1 __asm__("a1") = pid;
	__asm__ volatile("ecall" : "+r"(t0), "+r"(a0) : "r"(a1));
	return t0;
}

s3k_err_t s3k_try_mon_resume(uint64_t mon, uint64_t pid)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_MON_RESUME;
	register uint64_t a0 __asm__("a0") = mon;
	register uint64_t a1 __asm__("a1") = pid;
	__asm__ volatile("ecall" : "+r"(t0), "+r"(a0) : "r"(a1));
	return t0;
}

s3k_err_t s3k_try_mon_reg_read(uint64_t mon, uint64_t pid, uint64_t reg,
			       uint64_t *val)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_MON_REG_READ;
	register uint64_t a0 __asm__("a0") = mon;
	register uint64_t a1 __asm__("a1") = pid;
	register uint64_t a2 __asm__("a2") = reg;
	__asm__ volatile("ecall" : "+r"(t0), "+r"(a0) : "r"(a1), "r"(a2));
	*val = a0;
	return t0;
}

s3k_err_t s3k_try_mon_reg_write(uint64_t mon, uint64_t pid, uint64_t reg,
				uint64_t val)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_MON_REG_WRITE;
	register uint64_t a0 __asm__("a0") = mon;
	register uint64_t a1 __asm__("a1") = pid;
	register uint64_t a2 __asm__("a2") = reg;
	register uint64_t a3 __asm__("a3") = val;
	__asm__ volatile("ecall"
			 : "+r"(t0), "+r"(a0)
			 : "r"(a1), "r"(a2), "r"(a3));
	return t0;
}

s3k_err_t s3k_try_mon_cap_read(uint64_t mon, uint64_t pid, uint64_t idx,
			       s3k_cap_t *cap)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_MON_CAP_READ;
	register uint64_t a0 __asm__("a0") = mon;
	register uint64_t a1 __asm__("a1") = pid;
	register uint64_t a2 __asm__("a2") = idx;
	__asm__ volatile("ecall" : "+r"(t0), "+r"(a0) : "r"(a1), "r"(a2));
	if (!t0)
		cap->raw = a0;
	return t0;
}

s3k_err_t s3k_try_mon_cap_move(uint64_t mon, uint64_t src_pid, uint64_t src_idx,
			       uint64_t dst_pid, uint64_t dst_idx)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_MON_CAP_MOVE;
	register uint64_t a0 __asm__("a0") = mon;
	register uint64_t a1 __asm__("a1") = src_pid;
	register uint64_t a2 __asm__("a2") = src_idx;
	register uint64_t a3 __asm__("a3") = dst_pid;
	register uint64_t a4 __asm__("a4") = dst_idx;
	__asm__ volatile("ecall"
			 : "+r"(t0), "+r"(a0)
			 : "r"(a1), "r"(a2), "r"(a3), "r"(a4));
	return t0;
}

s3k_err_t s3k_try_mon_pmp_load(uint64_t mon, uint64_t pid, uint64_t idx,
			       uint64_t slot)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_MON_PMP_LOAD;
	register uint64_t a0 __asm__("a0") = mon;
	register uint64_t a1 __asm__("a1") = pid;
	register uint64_t a2 __asm__("a2") = idx;
	register uint64_t a3 __asm__("a3") = slot;
	__asm__ volatile("ecall"
			 : "+r"(t0), "+r"(a0)
			 : "r"(a1), "r"(a2), "r"(a3));
	return t0;
}

s3k_err_t s3k_try_mon_pmp_unload(uint64_t mon, uint64_t pid, uint64_t idx)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_MON_PMP_UNLOAD;
	register uint64_t a0 __asm__("a0") = mon;
	register uint64_t a1 __asm__("a1") = pid;
	register uint64_t a2 __asm__("a2") = idx;
	__asm__ volatile("ecall" : "+r"(t0), "+r"(a0) : "r"(a1), "r"(a2));
	return t0;
}

s3k_err_t s3k_try_sock_send(uint64_t sock_idx, uint64_t buf_idx,
			    uint64_t data[4], uint64_t send_cap)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_SOCK_SEND;
	register uint64_t a0 __asm__("a0") = sock_idx;
	register uint64_t a1 __asm__("a1") = buf_idx;
	register uint64_t a2 __asm__("a2") = data[0];
	register uint64_t a3 __asm__("a3") = data[1];
	register uint64_t a4 __asm__("a4") = data[2];
	register uint64_t a5 __asm__("a5") = data[3];
	register uint64_t a6 __asm__("a6") = send_cap;
	__asm__ volatile("ecall"
			 : "+r"(t0), "+r"(a0)
			 : "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5),
			   "r"(a6));
	return t0;
}

s3k_err_t s3k_try_sock_sendrecv(uint64_t sock_idx, uint64_t buf_idx,
				uint64_t data[4], uint64_t send_cap,
				uint64_t service_time)
{
	register uint64_t t0 __asm__("t0") = S3K_SYSCALL_SOCK_SENDRECV;
	register uint64_t a0 __asm__("a0") = sock_idx;
	register uint64_t a1 __asm__("a1") = buf_idx;
	register uint64_t a2 __asm__("a2") = data[0];
	register uint64_t a3 __asm__("a3") = data[1];
	register uint64_t a4 __asm__("a4") = data[2];
	register uint64_t a5 __asm__("a5") = data[3];
	register uint64_t a6 __asm__("a6") = send_cap;
	register uint64_t a7 __asm__("a7") = service_time;
	__asm__ volatile("ecall"
			 : "+r"(t0), "+r"(a0), "+r"(a1), "+r"(a2), "+r"(a3),
			   "+r"(a4), "+r"(a5)
			 : "r"(a6), "r"(a7));
	if (!t0) {
		data[0] = a2;
		data[1] = a3;
		data[2] = a4;
		data[3] = a5;
	}
	return t0;
}
