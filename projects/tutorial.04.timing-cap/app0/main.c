#include "altc/altio.h"
#include "s3k/s3k.h"

#define APP0_PID 0
#define APP1_PID 1

// See plat_conf.h
#define BOOT_PMP 0
#define RAM_MEM 1
#define UART_MEM 2
#define TIME_MEM 3
#define HART0_TIME 4
#define HART1_TIME 5
#define HART2_TIME 6
#define HART3_TIME 7
#define MONITOR 8
#define CHANNEL 9

#define CAP_UART 10
#define CAP_TMP	 11

#define APP_1_BASE_ADDR 0x80020000
#define APP_1_SIZE 0x10000
#define APP_1_PMP_MEM 0
#define APP_1_PMP_UART 1
#define APP_1_TIME 2

#define NO_APP_1 	0
#define NO_APP_0 	1
#define ROUND_ROBIN 2
#define PARALLEL	3

#define SCHEDULING NO_APP_0

void setup_uart(uint64_t uart_idx)
{
	uint64_t uart_addr = s3k_napot_encode(UART0_BASE_ADDR, 0x8);
	// Derive a PMP capability for accessing UART
	s3k_cap_derive(UART_MEM, uart_idx, s3k_mk_pmp(uart_addr, S3K_MEM_RW));
	// Load the derive PMP capability to PMP configuration
	s3k_pmp_load(uart_idx, 1);
	// Synchronize PMP unit (hardware) with PMP configuration
	// false => not full synchronization.
	s3k_sync_mem();
}

void setup_app_1(uint64_t tmp)
{
	uint64_t uart_addr = s3k_napot_encode(UART0_BASE_ADDR, 0x8);
	uint64_t app1_addr = s3k_napot_encode(APP_1_BASE_ADDR, APP_1_SIZE);

	// Derive a PMP capability for app1 main memory
	alt_printf("1 %X\n",
		s3k_cap_derive(RAM_MEM, tmp, s3k_mk_pmp(app1_addr, S3K_MEM_RWX))
			   );
	alt_printf("2 %X\n",
			   s3k_mon_cap_move(MONITOR, APP0_PID, tmp, APP1_PID, APP_1_PMP_MEM)
			   );
	alt_printf("3 %X\n",
			   s3k_mon_pmp_load(MONITOR, APP1_PID, APP_1_PMP_MEM, 0)
			   );

	// Derive a PMP capability for uart
	alt_printf("4 %X\n",
			   s3k_cap_derive(UART_MEM, tmp, s3k_mk_pmp(uart_addr, S3K_MEM_RW))
			   );

	alt_printf("5 %X\n",
			   s3k_mon_cap_move(MONITOR, APP0_PID, tmp, APP1_PID, APP_1_PMP_UART)
			   );
	alt_printf("6 %X\n",
			   s3k_mon_pmp_load(MONITOR, APP1_PID, APP_1_PMP_UART, 1)
			   );

	// Write start PC of app1 to PC
	alt_printf("9 %X\n",
			   s3k_mon_reg_write(MONITOR, APP1_PID, S3K_REG_PC, APP_1_BASE_ADDR)
			   );

	s3k_sync_mem();
}

void setup_scheduling(uint64_t tmp_cap_idx) {
	// Disable the other two cores
	s3k_cap_delete(HART2_TIME);
	s3k_cap_delete(HART3_TIME);

	if (SCHEDULING == NO_APP_1) {
		s3k_cap_delete(HART1_TIME);
	} 
	else if (SCHEDULING == NO_APP_0) {
		// Notice that we must be able to finish our job to setup APP 1
		s3k_cap_delete(HART1_TIME);
		s3k_cap_derive(HART0_TIME, tmp_cap_idx, s3k_mk_time(S3K_MIN_HART, 0, S3K_SLOT_CNT / 2));
		s3k_mon_cap_move(MONITOR, APP0_PID, HART0_TIME, APP1_PID, APP_1_TIME);
	}
	else if (SCHEDULING == ROUND_ROBIN) {
		s3k_cap_delete(HART1_TIME);
		alt_printf("Time derivation %d \n", 
			s3k_cap_derive(HART0_TIME, tmp_cap_idx, s3k_mk_time(S3K_MIN_HART, 0, 
												 S3K_SLOT_CNT / 2)));
		alt_printf("Time delegation %d \n", 
			s3k_mon_cap_move(MONITOR, APP0_PID, tmp_cap_idx, APP1_PID, APP_1_TIME));
	}
	else if (SCHEDULING == PARALLEL) {
		s3k_mon_cap_move(MONITOR, APP0_PID, HART1_TIME, APP1_PID, APP_1_TIME);
	}
}

int main(void)
{
	// Setup UART access
	setup_uart(CAP_UART);

	// Setup app1 capabilities and PC
	setup_app_1(CAP_TMP);

	// Write hello world.
	alt_puts("hello, world from app0");

	// Setup scehduling
	setup_scheduling(CAP_TMP);

	// Start app1
	alt_printf("10 %X\n", 
			   s3k_mon_resume(MONITOR, APP1_PID) 
			   ); 

	s3k_sync();

	while (1)
		alt_puts("0");


	// BYE!
	alt_puts("bye from app0");
}
