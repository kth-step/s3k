#pragma once
#include "s3k/types.h"

uint64_t s3k_get_pid(void);
uint64_t s3k_get_time(void);
uint64_t s3k_get_timeout(void);
uint64_t s3k_get_wcet(bool reset);
uint64_t s3k_reg_read(s3k_reg_t reg);
uint64_t s3k_reg_write(s3k_reg_t reg, uint64_t val);
void s3k_sync();
void s3k_sync_mem();
s3k_err_t s3k_cap_read(s3k_cidx_t idx, s3k_cap_t *cap);
s3k_err_t s3k_cap_move(s3k_cidx_t src, s3k_cidx_t dst);
s3k_err_t s3k_cap_delete(s3k_cidx_t idx);
s3k_err_t s3k_cap_revoke(s3k_cidx_t idx);
s3k_err_t s3k_cap_derive(s3k_cidx_t src, s3k_cidx_t dst, s3k_cap_t new_cap);
s3k_err_t s3k_pmp_load(s3k_cidx_t pmp_idx, s3k_pmp_slot_t pmp_slot);
s3k_err_t s3k_pmp_unload(s3k_cidx_t pmp_idx);
s3k_err_t s3k_mon_suspend(s3k_cidx_t mon_idx, s3k_pid_t pid);
s3k_err_t s3k_mon_resume(s3k_cidx_t mon_idx, s3k_pid_t pid);
s3k_err_t s3k_mon_state_get(s3k_cidx_t mon_idx, s3k_pid_t pid,
			    s3k_state_t *state);
s3k_err_t s3k_mon_yield(s3k_cidx_t mon_idx, s3k_pid_t pid);
s3k_err_t s3k_mon_reg_read(s3k_cidx_t mon_idx, s3k_pid_t pid, s3k_reg_t reg,
			   uint64_t *val);
s3k_err_t s3k_mon_reg_write(s3k_cidx_t mon_idx, s3k_pid_t pid, s3k_reg_t reg,
			    uint64_t val);
s3k_err_t s3k_mon_cap_read(s3k_cidx_t mon_idx, s3k_pid_t pid, s3k_cidx_t idx,
			   s3k_cap_t *cap);
s3k_err_t s3k_mon_cap_move(s3k_cidx_t mon_idx, s3k_pid_t src_pid,
			   s3k_cidx_t src_idx, s3k_pid_t dst_pid,
			   s3k_cidx_t dst_idx);
s3k_err_t s3k_mon_pmp_load(s3k_cidx_t mon_idx, s3k_pid_t pid,
			   s3k_cidx_t pmp_idx, s3k_pmp_slot_t pmp_slot);
s3k_err_t s3k_mon_pmp_unload(s3k_cidx_t mon_idx, s3k_pid_t pid,
			     s3k_cidx_t pmp_idx);
s3k_err_t s3k_sock_send(s3k_cidx_t sock_idx, const s3k_msg_t *msg);
s3k_reply_t s3k_sock_recv(s3k_cidx_t sock_idx, s3k_cidx_t cap_cidx);
s3k_reply_t s3k_sock_sendrecv(s3k_cidx_t sock_idx, const s3k_msg_t *msg);

s3k_err_t s3k_try_cap_move(s3k_cidx_t src, s3k_cidx_t dst);
s3k_err_t s3k_try_cap_delete(s3k_cidx_t idx);
s3k_err_t s3k_try_cap_revoke(s3k_cidx_t idx);
s3k_err_t s3k_try_cap_derive(s3k_cidx_t src, s3k_cidx_t dst, s3k_cap_t new_cap);
s3k_err_t s3k_try_pmp_load(s3k_cidx_t pmp_idx, s3k_pmp_slot_t pmp_slot);
s3k_err_t s3k_try_pmp_unload(s3k_cidx_t pmp_idx);
s3k_err_t s3k_try_mon_suspend(s3k_cidx_t mon_idx, s3k_pid_t pid);
s3k_err_t s3k_try_mon_resume(s3k_cidx_t mon_idx, s3k_pid_t pid);
s3k_err_t s3k_try_mon_state_get(s3k_cidx_t mon_idx, s3k_pid_t pid,
				s3k_state_t *state);
s3k_err_t s3k_try_mon_yield(s3k_cidx_t mon_idx, s3k_pid_t pid);
s3k_err_t s3k_try_mon_reg_read(s3k_cidx_t mon_idx, s3k_pid_t pid, s3k_reg_t reg,
			       uint64_t *val);
s3k_err_t s3k_try_mon_reg_write(s3k_cidx_t mon_idx, s3k_pid_t pid,
				s3k_reg_t reg, uint64_t val);
s3k_err_t s3k_try_mon_cap_read(s3k_cidx_t mon_idx, s3k_pid_t pid,
			       s3k_cidx_t idx, s3k_cap_t *cap);
s3k_err_t s3k_try_mon_cap_move(s3k_cidx_t mon_idx, s3k_pid_t src_pid,
			       s3k_cidx_t src_idx, s3k_pid_t dst_pid,
			       s3k_cidx_t dst_idx);
s3k_err_t s3k_try_mon_pmp_load(s3k_cidx_t mon_idx, s3k_pid_t pid,
			       s3k_cidx_t pmp_idx, s3k_pmp_slot_t pmp_slot);
s3k_err_t s3k_try_mon_pmp_unload(s3k_cidx_t mon_idx, s3k_pid_t pid,
				 s3k_cidx_t pmp_idx);
s3k_err_t s3k_try_sock_send(s3k_cidx_t sock_idx, const s3k_msg_t *msg);
s3k_reply_t s3k_try_sock_recv(s3k_cidx_t sock_idx, s3k_cidx_t cap_cidx);
s3k_reply_t s3k_try_sock_sendrecv(s3k_cidx_t sock_idx, const s3k_msg_t *msg);
