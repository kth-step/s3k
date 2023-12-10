#include "altc/altio.h"
#include "altc/string.h"
#include "s3k/s3k.h"

#define APP0_PID 0
#define APP1_PID 1

// See plat_conf.h
#define BOOT_PMP 0
#define RAM_MEM 1
#define UART_MEM 2
#define TIME_MEM 3
#define HART1_TIME 4
#define MONITOR 8
#define CHANNEL 9

void setup_uart(uint64_t uart_idx)
{
	uint64_t uart_addr = s3k_napot_encode(UART0_BASE_ADDR, 0x8);
	// Derive a PMP capability for accessing UART
	s3k_cap_derive(UART_MEM, uart_idx, s3k_mk_pmp(uart_addr, S3K_MEM_RW));
	// Load the derive PMP capability to PMP configuration
	s3k_pmp_load(uart_idx, 1);
	// Synchronize PMP unit (hardware) with PMP configuration
	s3k_sync_mem();
}

void setup_app1(uint64_t tmp)
{
	uint64_t uart_addr = s3k_napot_encode(UART0_BASE_ADDR, 0x8);
	uint64_t app1_addr = s3k_napot_encode(0x80020000, 0x10000);

	// Derive a PMP capability for app1 main memory
	s3k_cap_derive(RAM_MEM, tmp, s3k_mk_pmp(app1_addr, S3K_MEM_RWX));
	s3k_mon_cap_move(MONITOR, APP0_PID, tmp, APP1_PID, 0);
	s3k_mon_pmp_load(MONITOR, APP1_PID, 0, 0);

	// Derive a PMP capability for uart
	s3k_cap_derive(UART_MEM, tmp, s3k_mk_pmp(uart_addr, S3K_MEM_RW));
	s3k_mon_cap_move(MONITOR, APP0_PID, tmp, APP1_PID, 1);
	s3k_mon_pmp_load(MONITOR, APP1_PID, 1, 1);

	// Wait til start
	s3k_sleep(0);

	// give the time slice capability
	s3k_mon_cap_move(MONITOR, APP0_PID, HART1_TIME, APP1_PID, 2);

	s3k_mon_cap_move(MONITOR, APP0_PID, RAM_MEM, APP1_PID, 16);

	// Write start PC of app1 to PC
	s3k_mon_reg_write(MONITOR, APP1_PID, S3K_REG_PC, 0x80020000);
}

void setup_socket(uint64_t socket, uint64_t tmp)
{
	uint64_t perm = S3K_IPC_SDATA | S3K_IPC_CDATA;
#if defined(SCENARIO_TIME) || defined(SCENARIO_CAP)
	perm |= S3K_IPC_SCAP | S3K_IPC_CCAP;
#endif
	s3k_cap_derive(CHANNEL, socket,
		       s3k_mk_socket(0, S3K_IPC_YIELD, perm, 0));
	s3k_cap_derive(socket, tmp, s3k_mk_socket(0, S3K_IPC_YIELD, perm, 1));
	s3k_mon_cap_move(MONITOR, APP0_PID, tmp, APP1_PID, 3);
}

#define csrr(reg)                                                \
	({                                                       \
		register uint64_t __ret;                         \
		__asm__ volatile("csrr %0," #reg : "=r"(__ret)); \
		__ret;                                           \
	})

int main(void)
{
	setup_uart(10);

	alt_puts("starting app0");

	// Setup app1 capabilities and PC
	setup_app1(11);

	// Setup socket capabilities.
	setup_socket(11, 12);

	// Resume app1
	s3k_mon_resume(MONITOR, APP1_PID);

	s3k_msg_t msg = {0};
	s3k_reply_t reply;
	s3k_reg_write(S3K_REG_SERVTIME, 10);
	while (1) {
		do {
#if defined(SCENARIO_CAP) || defined(SCENARIO_TIME)
			msg.send_cap = 1;
			msg.cap_idx = 16;
#endif
			reply = s3k_sock_sendrecv(11, &msg);
		} while (reply.err);
		msg.data[0] = csrr(cycle);
	}
}
