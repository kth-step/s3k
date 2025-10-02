#include "s3k/syscall.h"

enum {
	S3K_SYSCALL_GET_PID,
	S3K_SYSCALL_SYNC,
	S3K_SYSCALL_SLEEP_UNTIL,
	S3K_SYSCALL_MEM_GET,
	S3K_SYSCALL_TSL_GET,
	S3K_SYSCALL_MON_GET,
	S3K_SYSCALL_IPC_GET,
	S3K_SYSCALL_MEM_DERIVE,
	S3K_SYSCALL_TSL_DERIVE,
	S3K_SYSCALL_MON_DERIVE,
	S3K_SYSCALL_IPC_DERIVE,
	S3K_SYSCALL_MEM_REVOKE,
	S3K_SYSCALL_TSL_REVOKE,
	S3K_SYSCALL_MON_REVOKE,
	S3K_SYSCALL_IPC_REVOKE,
	S3K_SYSCALL_MEM_DELETE,
	S3K_SYSCALL_TSL_DELETE,
	S3K_SYSCALL_MON_DELETE,
	S3K_SYSCALL_IPC_DELETE,
	S3K_SYSCALL_MEM_PMP_GET,
	S3K_SYSCALL_MEM_PMP_SET,
	S3K_SYSCALL_MEM_PMP_CLEAR,
	S3K_SYSCALL_TSL_SET,
	S3K_SYSCALL_MON_SUSPEND,
	S3K_SYSCALL_MON_RESUME,
	S3K_SYSCALL_MON_YIELD,
	S3K_SYSCALL_MON_REG_SET,
	S3K_SYSCALL_MON_REG_GET,
	S3K_SYSCALL_MON_MEM_GET,
	S3K_SYSCALL_MON_TSL_GET,
	S3K_SYSCALL_MON_MON_GET,
	S3K_SYSCALL_MON_IPC_GET,
	S3K_SYSCALL_MON_MEM_GRANT,
	S3K_SYSCALL_MON_TSL_GRANT,
	S3K_SYSCALL_MON_MON_GRANT,
	S3K_SYSCALL_MON_IPC_GRANT,
	S3K_SYSCALL_MON_MEM_DERIVE,
	S3K_SYSCALL_MON_TSL_DERIVE,
	S3K_SYSCALL_MON_MON_DERIVE,
	S3K_SYSCALL_MON_IPC_DERIVE,
	S3K_SYSCALL_MON_MEM_PMP_GET,
	S3K_SYSCALL_MON_MEM_PMP_SET,
	S3K_SYSCALL_MON_MEM_PMP_CLEAR,
	S3K_SYSCALL_MON_TSL_SET,
	S3K_SYSCALL_IPC_SEND,
	S3K_SYSCALL_IPC_RECV,
	S3K_SYSCALL_IPC_CALL,
	S3K_SYSCALL_IPC_REPLY,
	S3K_SYSCALL_IPC_REPLYRECV,
	S3K_SYSCALL_IPC_ASEND,
	S3K_SYSCALL_IPC_ARECV,
};

s3k_pid_t s3k_get_pid(void)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_GET_PID;
	__asm__ volatile("ecall" : "+r"(a0));
	return a0;
}

void s3k_sync(void)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_SYNC;
	__asm__ volatile("ecall" ::"r"(a0));
}

void s3k_sleep_until(s3k_time_t time)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_SLEEP_UNTIL;
	register s3k_word_t a1 __asm__("a1") = time;
	__asm__ volatile("ecall" ::"r"(a0), "r"(a1));
}

int s3k_mem_get(s3k_index_t i, s3k_cap_mem_t *cap)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_MEM_GET;
	register s3k_word_t a1 __asm__("a1") = i;
	register s3k_word_t a2 __asm__("a2");
	__asm__ volatile("ecall" : "+r"(a0), "+r"(a1), "=r"(a2));
	s3k_word_t *ptr = (s3k_word_t *)cap;
	ptr[0] = a1;
	ptr[1] = a2;
	return a0;
}

int s3k_tsl_get(s3k_index_t i, s3k_cap_tsl_t *cap)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_TSL_GET;
	register s3k_word_t a1 __asm__("a1") = i;
	register s3k_word_t a2 __asm__("a2");
	__asm__ volatile("ecall" : "+r"(a0), "+r"(a1), "=r"(a2));
	s3k_word_t *ptr = (s3k_word_t *)cap;
	ptr[0] = a1;
	ptr[1] = a2;
	return a0;
}

int s3k_mon_get(s3k_index_t i, s3k_cap_mon_t *cap)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_MON_GET;
	register s3k_word_t a1 __asm__("a1") = i;
	register s3k_word_t a2 __asm__("a2");
	__asm__ volatile("ecall" : "+r"(a0), "+r"(a1), "=r"(a2));
	s3k_word_t *ptr = (s3k_word_t *)cap;
	ptr[0] = a1;
	return a0;
}

int s3k_ipc_get(s3k_index_t i, s3k_cap_ipc_t *cap)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_IPC_GET;
	register s3k_word_t a1 __asm__("a1") = i;
	register s3k_word_t a2 __asm__("a2");
	__asm__ volatile("ecall" : "+r"(a0), "+r"(a1), "=r"(a2));
	s3k_word_t *ptr = (s3k_word_t *)cap;
	ptr[0] = a1;
	ptr[1] = a2;
	return a0;
}

int s3k_mem_derive(s3k_index_t i, s3k_fuel_t csize, s3k_mem_perm_t perm, s3k_mem_addr_t base, s3k_mem_addr_t size)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_MEM_DERIVE;
	register s3k_word_t a1 __asm__("a1") = i;
	register s3k_word_t a2 __asm__("a2") = csize;
	register s3k_word_t a3 __asm__("a3") = perm;
	register s3k_word_t a4 __asm__("a4") = base;
	register s3k_word_t a5 __asm__("a5") = size;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5));
	return a0;
}

int s3k_tsl_derive(s3k_index_t i, s3k_fuel_t csize, bool enabled, s3k_time_slot_t size)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_TSL_DERIVE;
	register s3k_word_t a1 __asm__("a1") = i;
	register s3k_word_t a2 __asm__("a2") = csize;
	register s3k_word_t a3 __asm__("a3") = enabled;
	register s3k_word_t a4 __asm__("a4") = size;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2), "r"(a3), "r"(a4));
	return a0;
}

int s3k_mon_derive(s3k_index_t i, s3k_fuel_t csize)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_MON_DERIVE;
	register s3k_word_t a1 __asm__("a1") = i;
	register s3k_word_t a2 __asm__("a2") = csize;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2));
	return a0;
}

int s3k_ipc_derive(s3k_index_t i, s3k_fuel_t csize, s3k_ipc_mode_t mode, s3k_ipc_flag_t flag)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_IPC_DERIVE;
	register s3k_word_t a1 __asm__("a1") = i;
	register s3k_word_t a2 __asm__("a2") = csize;
	register s3k_word_t a3 __asm__("a3") = mode;
	register s3k_word_t a4 __asm__("a4") = flag;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2), "r"(a3), "r"(a4));
	return a0;
}

int s3k_mem_revoke(s3k_index_t i)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_MEM_REVOKE;
	register s3k_word_t a1 __asm__("a1") = i;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1));
	return a0;
}

int s3k_tsl_revoke(s3k_index_t i)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_TSL_REVOKE;
	register s3k_word_t a1 __asm__("a1") = i;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1));
	return a0;
}

int s3k_mon_revoke(s3k_index_t i)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_MON_REVOKE;
	register s3k_word_t a1 __asm__("a1") = i;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1));
	return a0;
}

int s3k_ipc_revoke(s3k_index_t i)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_IPC_REVOKE;
	register s3k_word_t a1 __asm__("a1") = i;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1));
	return a0;
}

int s3k_mem_delete(s3k_index_t i)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_MEM_DELETE;
	register s3k_word_t a1 __asm__("a1") = i;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1));
	return a0;
}

int s3k_tsl_delete(s3k_index_t i)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_TSL_DELETE;
	register s3k_word_t a1 __asm__("a1") = i;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1));
	return a0;
}

int s3k_mon_delete(s3k_index_t i)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_MON_DELETE;
	register s3k_word_t a1 __asm__("a1") = i;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1));
	return a0;
}

int s3k_ipc_delete(s3k_index_t i)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_IPC_DELETE;
	register s3k_word_t a1 __asm__("a1") = i;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1));
	return a0;
}

int s3k_mem_pmp_get(s3k_index_t i, s3k_pmp_slot_t *slot, s3k_mem_perm_t *perm, s3k_pmp_addr_t *addr)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_MEM_PMP_GET;
	register s3k_word_t a1 __asm__("a1") = i;
	register s3k_word_t a2 __asm__("a2");
	register s3k_word_t a3 __asm__("a3");
	__asm__ volatile("ecall" : "+r"(a0), "+r"(a1), "=r"(a2), "=r"(a3));
	*slot = a1;
	*perm = a2;
	*addr = a3;
	return a0;
}

int s3k_mem_pmp_set(s3k_index_t i, s3k_pmp_slot_t slot, s3k_mem_perm_t perm, s3k_pmp_addr_t addr)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_MEM_PMP_SET;
	register s3k_word_t a1 __asm__("a1") = i;
	register s3k_word_t a2 __asm__("a2") = slot;
	register s3k_word_t a3 __asm__("a3") = perm;
	register s3k_word_t a4 __asm__("a4") = addr;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2), "r"(a3), "r"(a4));
	return a0;
}

int s3k_mem_pmp_clear(s3k_index_t i)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_MEM_PMP_CLEAR;
	register s3k_word_t a1 __asm__("a1") = i;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1));
	return a0;
}

int s3k_tsl_set(s3k_index_t i, bool enabled)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_TSL_SET;
	register s3k_word_t a1 __asm__("a1") = i;
	register s3k_word_t a2 __asm__("a2") = enabled;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2));
	return a0;
}

int s3k_mon_suspend(s3k_index_t i)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_MON_SUSPEND;
	register s3k_word_t a1 __asm__("a1") = i;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1));
	return a0;
}

int s3k_mon_resume(s3k_index_t i)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_MON_RESUME;
	register s3k_word_t a1 __asm__("a1") = i;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1));
	return a0;
}

int s3k_mon_yield(s3k_index_t i)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_MON_YIELD;
	register s3k_word_t a1 __asm__("a1") = i;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1));
	return a0;
}

int s3k_mon_reg_set(s3k_index_t i, s3k_reg_t reg, s3k_word_t value)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_MON_REG_SET;
	register s3k_word_t a1 __asm__("a1") = i;
	register s3k_word_t a2 __asm__("a2") = reg;
	register s3k_word_t a3 __asm__("a3") = value;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2), "r"(a3));
	return a0;
}

int s3k_mon_reg_get(s3k_index_t i, s3k_reg_t reg, s3k_word_t *value)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_MON_REG_GET;
	register s3k_word_t a1 __asm__("a1") = i;
	register s3k_word_t a2 __asm__("a2") = reg;
	__asm__ volatile("ecall" : "+r"(a0), "+r"(a1) : "r"(a2));
	*value = a1;
	return a0;
}

int s3k_mon_mem_get(s3k_index_t i, s3k_index_t j, s3k_cap_mem_t *cap)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_MON_MEM_GET;
	register s3k_word_t a1 __asm__("a1") = i;
	register s3k_word_t a2 __asm__("a2") = j;
	__asm__ volatile("ecall" : "+r"(a0), "+r"(a1), "+r"(a2));
	s3k_word_t *ptr = (s3k_word_t *)cap;
	ptr[0] = a1;
	ptr[1] = a2;
	return a0;
}

int s3k_mon_tsl_get(s3k_index_t i, s3k_index_t j, s3k_cap_tsl_t *cap)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_MON_TSL_GET;
	register s3k_word_t a1 __asm__("a1") = i;
	register s3k_word_t a2 __asm__("a2") = j;
	__asm__ volatile("ecall" : "+r"(a0), "+r"(a1), "+r"(a2));
	s3k_word_t *ptr = (s3k_word_t *)cap;
	ptr[0] = a1;
	ptr[1] = a2;
	return a0;
}

int s3k_mon_mon_get(s3k_index_t i, s3k_index_t j, s3k_cap_mon_t *cap)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_MON_MON_GET;
	register s3k_word_t a1 __asm__("a1") = i;
	register s3k_word_t a2 __asm__("a2") = j;
	__asm__ volatile("ecall" : "+r"(a0), "+r"(a1) : "r"(a2));
	s3k_word_t *ptr = (s3k_word_t *)cap;
	ptr[0] = a1;
	return a0;
}

int s3k_mon_ipc_get(s3k_index_t i, s3k_index_t j, s3k_cap_ipc_t *cap)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_MON_IPC_GET;
	register s3k_word_t a1 __asm__("a1") = i;
	register s3k_word_t a2 __asm__("a2") = j;
	__asm__ volatile("ecall" : "+r"(a0), "+r"(a1), "+r"(a2));
	s3k_word_t *ptr = (s3k_word_t *)cap;
	ptr[0] = a1;
	ptr[1] = a2;
	return a0;
}

int s3k_mon_mem_grant(s3k_index_t i, s3k_index_t j)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_MON_MEM_GRANT;
	register s3k_word_t a1 __asm__("a1") = i;
	register s3k_word_t a2 __asm__("a2") = j;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2));
	return a0;
}

int s3k_mon_tsl_grant(s3k_index_t i, s3k_index_t j)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_MON_TSL_GRANT;
	register s3k_word_t a1 __asm__("a1") = i;
	register s3k_word_t a2 __asm__("a2") = j;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2));
	return a0;
}

int s3k_mon_mon_grant(s3k_index_t i, s3k_index_t j)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_MON_MON_GRANT;
	register s3k_word_t a1 __asm__("a1") = i;
	register s3k_word_t a2 __asm__("a2") = j;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2));
	return a0;
}

int s3k_mon_ipc_grant(s3k_index_t i, s3k_index_t j)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_MON_IPC_GRANT;
	register s3k_word_t a1 __asm__("a1") = i;
	register s3k_word_t a2 __asm__("a2") = j;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2));
	return a0;
}

int s3k_mon_mem_derive(s3k_index_t i, s3k_index_t j, s3k_fuel_t csize, s3k_mem_perm_t perm, s3k_mem_addr_t base,
		       s3k_mem_addr_t size)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_MON_MEM_DERIVE;
	register s3k_word_t a1 __asm__("a1") = i;
	register s3k_word_t a2 __asm__("a2") = j;
	register s3k_word_t a3 __asm__("a3") = csize;
	register s3k_word_t a4 __asm__("a4") = perm;
	register s3k_word_t a5 __asm__("a5") = base;
	register s3k_word_t a6 __asm__("a6") = size;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6));
	return a0;
}

int s3k_mon_tsl_derive(s3k_index_t i, s3k_index_t j, s3k_fuel_t csize, bool enabled, s3k_time_slot_t size)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_MON_TSL_DERIVE;
	register s3k_word_t a1 __asm__("a1") = i;
	register s3k_word_t a2 __asm__("a2") = j;
	register s3k_word_t a3 __asm__("a3") = csize;
	register s3k_word_t a4 __asm__("a4") = enabled;
	register s3k_word_t a5 __asm__("a5") = size;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5));
	return a0;
}

int s3k_mon_mon_derive(s3k_index_t i, s3k_index_t j, s3k_fuel_t csize)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_MON_MON_DERIVE;
	register s3k_word_t a1 __asm__("a1") = i;
	register s3k_word_t a2 __asm__("a2") = j;
	register s3k_word_t a3 __asm__("a3") = csize;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2), "r"(a3));
	return a0;
}

int s3k_mon_ipc_derive(s3k_index_t i, s3k_index_t j, s3k_fuel_t csize, s3k_ipc_mode_t mode, s3k_ipc_flag_t flag)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_MON_IPC_DERIVE;
	register s3k_word_t a1 __asm__("a1") = i;
	register s3k_word_t a2 __asm__("a2") = j;
	register s3k_word_t a3 __asm__("a3") = csize;
	register s3k_word_t a4 __asm__("a4") = mode;
	register s3k_word_t a5 __asm__("a5") = flag;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5));
	return a0;
}

int s3k_mon_mem_pmp_get(s3k_index_t i, s3k_index_t j, s3k_pmp_slot_t *slot, s3k_mem_perm_t *perm, s3k_pmp_addr_t *addr)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_MON_MEM_PMP_GET;
	register s3k_word_t a1 __asm__("a1") = i;
	register s3k_word_t a2 __asm__("a2") = j;
	register s3k_word_t a3 __asm__("a3");
	__asm__ volatile("ecall" : "+r"(a0), "+r"(a1), "+r"(a2), "=r"(a3));
	*slot = a1;
	*perm = a2;
	*addr = a3;
	return a0;
}

int s3k_mon_mem_pmp_set(s3k_index_t i, s3k_index_t j, s3k_pmp_slot_t slot, s3k_mem_perm_t perm, s3k_pmp_addr_t addr)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_MON_MEM_PMP_SET;
	register s3k_word_t a1 __asm__("a1") = i;
	register s3k_word_t a2 __asm__("a2") = j;
	register s3k_word_t a3 __asm__("a3") = slot;
	register s3k_word_t a4 __asm__("a4") = perm;
	register s3k_word_t a5 __asm__("a5") = addr;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5));
	return a0;
}

int s3k_mon_mem_pmp_clear(s3k_index_t i, s3k_index_t j)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_MON_MEM_PMP_CLEAR;
	register s3k_word_t a1 __asm__("a1") = i;
	register s3k_word_t a2 __asm__("a2") = j;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2));
	return a0;
}

int s3k_mon_tsl_set(s3k_index_t i, s3k_index_t j, bool enabled)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_MON_TSL_SET;
	register s3k_word_t a1 __asm__("a1") = i;
	register s3k_word_t a2 __asm__("a2") = j;
	register s3k_word_t a3 __asm__("a3") = enabled;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2), "r"(a3));
	return a0;
}

int s3k_ipc_send(s3k_index_t i, s3k_word_t msg[2], s3k_capty_t capty, s3k_index_t j)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_IPC_SEND;
	register s3k_word_t a1 __asm__("a1") = i;
	register s3k_word_t a2 __asm__("a2") = msg[0];
	register s3k_word_t a3 __asm__("a3") = msg[1];
	register s3k_word_t a4 __asm__("a4") = capty;
	register s3k_word_t a5 __asm__("a5") = j;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5));
	return a0;
}

int s3k_ipc_recv(s3k_index_t i, s3k_word_t msg[2], s3k_capty_t *capty, s3k_index_t *j)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_IPC_RECV;
	register s3k_word_t a1 __asm__("a1") = i;
	register s3k_word_t a2 __asm__("a2");
	register s3k_word_t a3 __asm__("a3");
	register s3k_word_t a4 __asm__("a4");
	__asm__ volatile("ecall" : "+r"(a0), "+r"(a1), "=r"(a2), "=r"(a3), "=r"(a4));
	msg[0] = a1;
	msg[1] = a2;
	*capty = a3;
	*j = a4;
	return a0;
}

int s3k_ipc_call(s3k_index_t i, s3k_word_t msg[2], s3k_capty_t *capty, s3k_index_t *j)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_IPC_CALL;
	register s3k_word_t a1 __asm__("a1") = i;
	register s3k_word_t a2 __asm__("a2") = msg[0];
	register s3k_word_t a3 __asm__("a3") = msg[1];
	register s3k_word_t a4 __asm__("a4") = *capty;
	register s3k_word_t a5 __asm__("a5") = *j;
	__asm__ volatile("ecall" : "+r"(a0), "+r"(a1), "+r"(a2), "+r"(a3), "+r"(a4) : "r"(a5));
	msg[0] = a1;
	msg[1] = a2;
	*capty = a3;
	*j = a4;
	return a0;
}

int s3k_ipc_reply(s3k_index_t i, s3k_word_t msg[2], s3k_capty_t capty, s3k_index_t j)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_IPC_REPLY;
	register s3k_word_t a1 __asm__("a1") = i;
	register s3k_word_t a2 __asm__("a2") = msg[0];
	register s3k_word_t a3 __asm__("a3") = msg[1];
	register s3k_word_t a4 __asm__("a4") = capty;
	register s3k_word_t a5 __asm__("a5") = j;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5));
	return a0;
}

int s3k_ipc_replyrecv(s3k_index_t i, s3k_word_t msg[2], s3k_capty_t *capty, s3k_index_t *j)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_IPC_REPLYRECV;
	register s3k_word_t a1 __asm__("a1") = i;
	register s3k_word_t a2 __asm__("a2") = msg[0];
	register s3k_word_t a3 __asm__("a3") = msg[1];
	register s3k_word_t a4 __asm__("a4") = *capty;
	register s3k_word_t a5 __asm__("a5") = *j;
	__asm__ volatile("ecall" : "+r"(a0), "+r"(a1), "+r"(a2), "+r"(a3), "+r"(a4) : "r"(a5));
	msg[0] = a1;
	msg[1] = a2;
	*capty = a3;
	*j = a4;
	return a0;
}

int s3k_ipc_asend(s3k_index_t i, s3k_word_t msg)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_IPC_ASEND;
	register s3k_word_t a1 __asm__("a1") = i;
	register s3k_word_t a2 __asm__("a2") = msg;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2));
	return a0;
}

int s3k_ipc_arecv(s3k_index_t i, s3k_word_t *msg)
{
	register s3k_word_t a0 __asm__("a0") = S3K_SYSCALL_IPC_ARECV;
	register s3k_word_t a1 __asm__("a1") = i;
	__asm__ volatile("ecall" : "+r"(a0), "+r"(a1));
	*msg = a1;
	return a0;
}
