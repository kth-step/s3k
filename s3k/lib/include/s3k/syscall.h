#ifndef S3K_SYSCALL_H
#define S3K_SYSCALL_H

#include "s3k/types.h"

s3k_pid_t s3k_get_pid(void);
void s3k_sync(void);
void s3k_sleep_until(s3k_time_t time);

int s3k_mem_get(s3k_index_t i, s3k_cap_mem_t *cap);
int s3k_tsl_get(s3k_index_t i, s3k_cap_tsl_t *cap);
int s3k_mon_get(s3k_index_t i, s3k_cap_mon_t *cap);
int s3k_ipc_get(s3k_index_t i, s3k_cap_ipc_t *cap);
int s3k_mem_derive(s3k_index_t i, s3k_fuel_t cfree, s3k_mem_perm_t perm, s3k_mem_addr_t begin, s3k_mem_addr_t end);
int s3k_tsl_derive(s3k_index_t i, s3k_fuel_t cfree, bool enabled, s3k_time_slot_t length);
int s3k_mon_derive(s3k_index_t i, s3k_fuel_t cfree);
int s3k_ipc_derive(s3k_index_t i, s3k_fuel_t cfree, s3k_ipc_mode_t mode, s3k_ipc_flag_t flag);
int s3k_mem_revoke(s3k_index_t i);
int s3k_tsl_revoke(s3k_index_t i);
int s3k_mon_revoke(s3k_index_t i);
int s3k_ipc_revoke(s3k_index_t i);
int s3k_mem_delete(s3k_index_t i);
int s3k_tsl_delete(s3k_index_t i);
int s3k_mon_delete(s3k_index_t i);
int s3k_ipc_delete(s3k_index_t i);
int s3k_mem_pmp_get(s3k_index_t i, s3k_pmp_slot_t *slot, s3k_mem_perm_t *perm, s3k_pmp_addr_t *addr);
int s3k_mem_pmp_set(s3k_index_t i, s3k_pmp_slot_t slot, s3k_mem_perm_t perm, s3k_pmp_addr_t addr);
int s3k_mem_pmp_clear(s3k_index_t i);
int s3k_tsl_set(s3k_index_t i, bool enabled);
int s3k_mon_suspend(s3k_index_t i);
int s3k_mon_resume(s3k_index_t i);
int s3k_mon_yield(s3k_index_t i);
int s3k_mon_reg_set(s3k_index_t i, s3k_reg_t reg, s3k_word_t val);
int s3k_mon_reg_get(s3k_index_t i, s3k_reg_t reg, s3k_word_t *val);
int s3k_mon_mem_get(s3k_index_t i, s3k_index_t j, s3k_cap_mem_t *cap);
int s3k_mon_tsl_get(s3k_index_t i, s3k_index_t j, s3k_cap_tsl_t *cap);
int s3k_mon_mon_get(s3k_index_t i, s3k_index_t j, s3k_cap_mon_t *cap);
int s3k_mon_ipc_get(s3k_index_t i, s3k_index_t j, s3k_cap_ipc_t *cap);
int s3k_mon_mem_grant(s3k_index_t i, s3k_index_t j);
int s3k_mon_tsl_grant(s3k_index_t i, s3k_index_t j);
int s3k_mon_mon_grant(s3k_index_t i, s3k_index_t j);
int s3k_mon_ipc_grant(s3k_index_t i, s3k_index_t j);
int s3k_mon_mem_derive(s3k_index_t i, s3k_index_t j, s3k_fuel_t cfree, s3k_mem_perm_t perm, s3k_mem_addr_t begin,
		       s3k_mem_addr_t end);
int s3k_mon_tsl_derive(s3k_index_t i, s3k_index_t j, s3k_fuel_t cfree, bool enabled, s3k_time_slot_t length);
int s3k_mon_mon_derive(s3k_index_t i, s3k_index_t j, s3k_fuel_t cfree);
int s3k_mon_ipc_derive(s3k_index_t i, s3k_index_t j, s3k_fuel_t cfree, s3k_ipc_mode_t mode, s3k_ipc_flag_t flag);
int s3k_mon_mem_pmp_get(s3k_index_t i, s3k_index_t j, s3k_pmp_slot_t *slot, s3k_mem_perm_t *perm, s3k_pmp_addr_t *addr);
int s3k_mon_mem_pmp_set(s3k_index_t i, s3k_index_t j, s3k_pmp_slot_t slot, s3k_mem_perm_t perm, s3k_pmp_addr_t addr);
int s3k_mon_mem_pmp_clear(s3k_index_t i, s3k_index_t j);
int s3k_mon_tsl_set(s3k_index_t i, s3k_index_t j, bool enabled);
int s3k_ipc_send(s3k_index_t i, s3k_word_t msg[2], s3k_capty_t capty, s3k_index_t j);
int s3k_ipc_recv(s3k_index_t i, s3k_word_t msg[2], s3k_capty_t *capty, s3k_index_t *j);
int s3k_ipc_call(s3k_index_t i, s3k_word_t msg[2], s3k_capty_t *capty, s3k_index_t *j);
int s3k_ipc_reply(s3k_index_t i, s3k_word_t msg[2], s3k_capty_t capty, s3k_index_t j);
int s3k_ipc_replyrecv(s3k_index_t i, s3k_word_t msg[2], s3k_capty_t *capty, s3k_index_t *j);
int s3k_ipc_asend(s3k_index_t i, s3k_word_t msg);
int s3k_ipc_arecv(s3k_index_t i, s3k_word_t *msg);

#endif // S3K_SYSCALL_H
