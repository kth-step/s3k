#include "../config.h"
#include "altc/altio.h"
#include "s3k/s3k.h"
#include "string.h"

#include <stddef.h>

// Defined in payload.S
extern char monitor_bin[];
extern char crypto_bin[];
extern char uart_bin[];
extern size_t monitor_bin_len;
extern size_t crypto_bin_len;
extern size_t uart_bin_len;

// From plat_conf.h
#define PMP_BOOT_CIDX 0
#define MEMORY_RAM_CIDX 1
#define MEMORY_UART_CIDX 2
#define MEMORY_TIME_CIDX 3
#define TIME_HART0_CIDX 4
#define TIME_HART1_CIDX 5
#define TIME_HART2_CIDX 6
#define TIME_HART3_CIDX 7
#define MONITOR_CIDX 8
#define CHANNEL_CIDX 9

s3k_err_t mon_grant_cap(s3k_pid_t pid, s3k_cidx_t src, s3k_cidx_t dst)
{
	return s3k_mon_cap_move(MONITOR_CIDX, BOOT_PID, src, pid, dst);
}

s3k_err_t mon_pmp_load(s3k_pid_t pid, s3k_cidx_t idx, s3k_pmp_slot_t slot)
{
	return s3k_mon_pmp_load(MONITOR_CIDX, pid, idx, slot);
}

s3k_err_t mon_write_reg(s3k_pid_t pid, s3k_reg_t regnr, uint64_t val)
{
	return s3k_mon_reg_write(MONITOR_CIDX, pid, regnr, val);
}

s3k_err_t mon_resume(s3k_pid_t pid)
{
	return s3k_mon_resume(MONITOR_CIDX, pid);
}

s3k_err_t derive_ram(s3k_cidx_t dst, char *base, size_t len, s3k_mem_perm_t rwx)
{
	s3k_cap_t cap
	    = s3k_mk_memory((uint64_t)base, (uint64_t)base + len, rwx);
	return s3k_cap_derive(MEMORY_RAM_CIDX, dst, cap);
}

s3k_err_t derive_pmp(s3k_cidx_t src, s3k_cidx_t dst, char *base, size_t len,
		     s3k_mem_perm_t rwx)
{
	s3k_napot_t addr = s3k_napot_encode((s3k_addr_t)base, len);
	s3k_cap_t cap = s3k_mk_pmp(addr, rwx);
	return s3k_cap_derive(src, dst, cap);
}

void setup_bin(s3k_pid_t pid, char *mem, size_t mem_len, char *bin,
	       size_t bin_len)
{
	derive_ram(0x10, mem, mem_len, S3K_MEM_RWX);
	derive_pmp(0x10, 0x11, mem, mem_len, S3K_MEM_RWX);
	s3k_pmp_load(0x11, 2);
	s3k_sync_mem();
	memcpy(mem, bin, bin_len);
	mon_grant_cap(pid, 0x10, 1);
	mon_grant_cap(pid, 0x11, 0);
	mon_pmp_load(pid, 0, 0);
	mon_write_reg(pid, S3K_REG_PC, (uint64_t)mem);
	s3k_sync_mem();
}

void setup_shared_mem(void)
{
	derive_ram(0x10, SHARED0_MEM, SHARED0_MEM_LEN, S3K_MEM_RW);
	derive_pmp(0x10, 0x11, SHARED0_MEM, SHARED0_MEM_LEN, S3K_MEM_RW);
	derive_pmp(0x10, 0x12, SHARED0_MEM, SHARED0_MEM_LEN, S3K_MEM_RW);
	s3k_cap_delete(0x10);
	mon_grant_cap(MONITOR_PID, 0x11, 0x3);
	mon_grant_cap(UART_PID, 0x12, 0x3);
	mon_pmp_load(MONITOR_PID, 0x3, 0x1);
	mon_pmp_load(UART_PID, 0x3, 0x1);

	derive_ram(0x10, SHARED1_MEM, SHARED1_MEM_LEN, S3K_MEM_RW);
	derive_pmp(0x10, 0x11, SHARED1_MEM, SHARED1_MEM_LEN, S3K_MEM_RW);
	derive_pmp(0x10, 0x12, SHARED1_MEM, SHARED1_MEM_LEN, S3K_MEM_RW);
	s3k_cap_delete(0x10);
	mon_grant_cap(MONITOR_PID, 0x11, 0x4);
	mon_grant_cap(CRYPTO_PID, 0x12, 0x2);
	mon_pmp_load(MONITOR_PID, 0x4, 0x1);
	mon_pmp_load(CRYPTO_PID, 0x2, 0x1);
}

void setup_uart_pmp(void)
{
	derive_pmp(MEMORY_UART_CIDX, 0x10, (void *)UART0_BASE_ADDR, 0x1000,
		   S3K_MEM_RW);
	mon_grant_cap(UART_PID, 0x10, 0x4);
	mon_pmp_load(UART_PID, 0x4, 0x2);
}

void setup_app_mem(void)
{
	derive_ram(0x10, APP0_MEM, APP0_MEM_LEN, S3K_MEM_RWX);
	derive_ram(0x11, APP1_MEM, APP1_MEM_LEN, S3K_MEM_RWX);
	mon_grant_cap(MONITOR_PID, 0x10, 0x5);
	mon_grant_cap(MONITOR_PID, 0x11, 0x6);
}

void setup_uart_ipc(s3k_chan_t chan, s3k_ipc_mode_t mode, s3k_ipc_perm_t perm)
{
	s3k_cap_derive(CHANNEL_CIDX, 0x10, s3k_mk_socket(chan, mode, perm, 0));
	s3k_cap_derive(0x10, 0x11, s3k_mk_socket(chan, mode, perm, 1));
	mon_grant_cap(MONITOR_PID, 0x10, 0x7);
	mon_grant_cap(UART_PID, 0x11, 0x5);
}

void setup_crypto_ipc(s3k_chan_t chan, s3k_ipc_mode_t mode, s3k_ipc_perm_t perm)
{
	s3k_cap_derive(CHANNEL_CIDX, 0x10, s3k_mk_socket(chan, mode, perm, 0));
	s3k_cap_derive(0x10, 0x11, s3k_mk_socket(chan, mode, perm, 1));
	mon_grant_cap(CRYPTO_PID, 0x10, 0x3);
	mon_grant_cap(MONITOR_PID, 0x11, 0x8);
}

void setup_app_channels(void)
{
	s3k_cap_derive(CHANNEL_CIDX, 0x10, s3k_mk_channel(2, 4));
	mon_grant_cap(MONITOR_PID, 0x10, 0x9);
}

void setup_app_monitoring(void)
{
	// Monitor for boot, monitor, crypto, uart
	s3k_cap_derive(MONITOR_CIDX, 0x10, s3k_mk_monitor(0, 4));
	// Monitor for app0, app1
	s3k_cap_derive(MONITOR_CIDX, 0x11, s3k_mk_monitor(4, 6));
	s3k_cap_delete(MONITOR_CIDX);
	s3k_cap_move(0x10, MONITOR_CIDX);
	mon_grant_cap(MONITOR_PID, 0x11, 0xa);
}

void setup_time(void)
{
	s3k_cap_derive(TIME_HART0_CIDX, 0x10,
		       s3k_mk_time(S3K_MIN_HART, 0, UART_TIME));
	s3k_cap_derive(TIME_HART0_CIDX, 0x11,
		       s3k_mk_time(S3K_MIN_HART, UART_TIME, S3K_SLOT_CNT));
	s3k_cap_derive(0x10, 0x12, s3k_mk_time(S3K_MIN_HART, 0, UART_TIME - 2));
	s3k_cap_derive(0x11, 0x13,
		       s3k_mk_time(S3K_MIN_HART, UART_TIME, S3K_SLOT_CNT - 2));

	mon_grant_cap(UART_PID, 0x10, 0x2);
	mon_grant_cap(MONITOR_PID, 0x11, 0x2);
}

void start_proc(void)
{
	s3k_mon_resume(MONITOR_CIDX, CRYPTO_PID);
	while (s3k_mon_yield(MONITOR_CIDX, CRYPTO_PID))
		;
	alt_puts("crypto started");

	s3k_mon_resume(MONITOR_CIDX, MONITOR_PID);
	while (s3k_mon_yield(MONITOR_CIDX, MONITOR_PID))
		;
	alt_puts("monitor started");

	s3k_mon_resume(MONITOR_CIDX, UART_PID);
	while (s3k_mon_yield(MONITOR_CIDX, UART_PID))
		;
	// UART outputs "uart started"
	alt_puts("error");
	while (1)
		;
}

void trap_handler(void) __attribute__((interrupt("machine")));

void trap_handler(void)
{
}

void main(void)
{
	s3k_reg_write(S3K_REG_TPC, (uint64_t)trap_handler);
	s3k_cap_delete(TIME_HART1_CIDX);
	s3k_cap_delete(TIME_HART2_CIDX);
	s3k_cap_delete(TIME_HART3_CIDX);
	derive_pmp(MEMORY_UART_CIDX, 0x5, (void *)UART0_BASE_ADDR, 0x1000,
		   S3K_MEM_RW);
	s3k_pmp_load(5, 1);
	s3k_sync_mem();

	alt_puts("setting up memory ...");
	/* Copy binary of monitor process, setup PMP and program counter. */
	setup_bin(MONITOR_PID, MONITOR_MEM, MONITOR_MEM_LEN, monitor_bin,
		  monitor_bin_len);
	/* Copy binary of crypto process, setup PMP and program counter. */
	setup_bin(CRYPTO_PID, CRYPTO_MEM, CRYPTO_MEM_LEN, crypto_bin,
		  crypto_bin_len);
	/* Copy binary of uart process, setup PMP and program counter. */
	setup_bin(UART_PID, UART_MEM, UART_MEM_LEN, uart_bin, uart_bin_len);
	/* Give UART and Monitor processes shared memory */
	setup_shared_mem();
	/* Give UART process access to uart */
	setup_uart_pmp();
	/* Give monitor access to application memory */
	setup_app_mem();

	alt_puts("setting up ipc");
	setup_uart_ipc(0, S3K_IPC_NOYIELD, S3K_IPC_CDATA | S3K_IPC_SDATA);
	setup_crypto_ipc(1, S3K_IPC_NOYIELD, S3K_IPC_CDATA | S3K_IPC_SDATA);
	setup_app_channels();
	setup_app_monitoring();

	alt_puts("setting up time");
	setup_time();

	alt_puts("starting processes");
	start_proc();
}
