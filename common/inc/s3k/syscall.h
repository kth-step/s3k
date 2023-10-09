#pragma once
#include "s3k/types.h"

typedef enum {
	// Basic Info & Registers
	S3K_SYS_GET_INFO,  // Retrieve system information
	S3K_SYS_REG_READ,  // Read from a register
	S3K_SYS_REG_WRITE, // Write to a register
	S3K_SYS_SYNC,	   // Synchronize memory and time.

	// Capability Management
	S3K_SYS_CAP_READ,   // Read the properties of a capability.
	S3K_SYS_CAP_MOVE,   // Move a capability to a different slot.
	S3K_SYS_CAP_DELETE, // Delete a capability from the system.
	S3K_SYS_CAP_REVOKE, // Deletes derived capabilities.
	S3K_SYS_CAP_DERIVE, // Creates a new capability.

	// PMP calls
	S3K_SYS_PMP_LOAD,
	S3K_SYS_PMP_UNLOAD,

	// Monitor calls
	S3K_SYS_MON_SUSPEND,
	S3K_SYS_MON_RESUME,
	S3K_SYS_MON_STATE_GET,
	S3K_SYS_MON_YIELD,
	S3K_SYS_MON_REG_READ,
	S3K_SYS_MON_REG_WRITE,
	S3K_SYS_MON_CAP_READ,
	S3K_SYS_MON_CAP_MOVE,
	S3K_SYS_MON_PMP_LOAD,
	S3K_SYS_MON_PMP_UNLOAD,

	// Socket calls
	S3K_SYS_SOCK_SEND,
	S3K_SYS_SOCK_SENDRECV,
} s3k_syscall_t;

uint64_t s3k_get_pid(void);
uint64_t s3k_get_time(void);
uint64_t s3k_get_timeout(void);
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
s3k_reply_t s3k_try_sock_sendrecv(s3k_cidx_t sock_idx, const s3k_msg_t *msg);
