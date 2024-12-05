#include "altc/altio.h"
#include "s3k/s3k.h"
#include "altc/string.h"

#include "ff.h"

#include "../../tutorial-commons/utils.h"

void setup_other_app()
{
	uint64_t uart_addr = s3k_napot_encode(UART0_BASE_ADDR, 0x8);
	uint64_t app1_addr = s3k_napot_encode(APP_1_BASE_ADDR, APP_1_SIZE);

	// Derive a PMP capability for app1 main memory
	uint32_t free_cap_mem_idx = find_free_cap();
	s3k_cap_derive(RAM_MEM, free_cap_mem_idx, s3k_mk_memory(APP_1_BASE_ADDR, APP_1_BASE_ADDR + APP_1_SIZE, S3K_MEM_RWX));
	uint32_t free_cap_idx = find_free_cap();
	s3k_cap_derive(free_cap_mem_idx, free_cap_idx, s3k_mk_pmp(app1_addr, S3K_MEM_RWX));
	s3k_mon_cap_move(MONITOR, APP0_PID, free_cap_idx, APP1_PID, APP_1_CAP_PMP_MEM);
	s3k_mon_pmp_load(MONITOR, APP1_PID, APP_1_CAP_PMP_MEM, APP_1_PMP_SLOT_MEM);

	// Keep a PMP for ourselves to reqrite APP1_memory
	free_cap_idx = find_free_cap();
	s3k_cap_derive(free_cap_mem_idx, free_cap_idx, s3k_mk_pmp(app1_addr, S3K_MEM_RWX));
	s3k_pmp_load(free_cap_idx, BUFFER_PMP);

	// Derive a PMP capability for uart
	free_cap_idx = find_free_cap();
	s3k_cap_derive(UART_MEM, free_cap_idx, s3k_mk_pmp(uart_addr, S3K_MEM_RW));
	s3k_mon_cap_move(MONITOR, APP0_PID, free_cap_idx, APP1_PID, APP_1_CAP_PMP_UART);
	s3k_mon_pmp_load(MONITOR, APP1_PID, APP_1_CAP_PMP_UART, APP_1_PMP_SLOT_UART);

	// Write start PC of app1 to PC
	s3k_mon_reg_write(MONITOR, APP1_PID, S3K_REG_PC, APP_1_BASE_ADDR);

	s3k_sync_mem();
}


int main(void)
{
	setup_uart_and_virtio();
	alt_puts("0> starting app");

	FATFS FatFs;					/* FatFs work area needed for each volume */
	f_mount(&FatFs, "", 0);			/* Give a work area to the default drive */
	alt_printf("0> File system mounted\n");

	// Setup app1 capabilities and PC
	setup_other_app();
	setup_scheduling(ROUND_ROBIN);
	uint32_t socket = setup_socket(true, true, false);

	// Resume app1
	s3k_mon_resume(MONITOR, APP1_PID);

	s3k_msg_t msg;
	s3k_reply_t reply;

	while (1) {
		do {
			reply = s3k_sock_sendrecv(socket, &msg);
		} while (reply.err);
		alt_puts("0> hello\n");
		alt_printf("0> file %s\n", (char *)reply.data);
		s3k_mon_suspend(MONITOR, APP1_PID);

		UINT bw;
		FRESULT fr;
		FIL Fil;									/* File object needed for each open file */

		char buffer[1024];
		fr = f_open(&Fil, (char *)reply.data, FA_READ);
		if (fr == FR_OK) {
			alt_puts("0> File opened \n");
			f_read(&Fil, buffer, 1024, &bw);	/*Read data from the file */
			fr = f_close(&Fil);							/* Close the file */
			if (fr == FR_OK) {
				alt_puts("0> Booting app1 \n");
				memcpy((void*)APP_1_BASE_ADDR,buffer, 1024);
				s3k_mon_reg_write(MONITOR, APP1_PID, S3K_REG_PC, APP_1_BASE_ADDR); //reset the PC to the initial address
				s3k_mon_resume(MONITOR, APP1_PID);
			}
			else {
				alt_puts("0> Filed to read file \n");
			}
		} else{
			alt_puts("0> File not opened\n");
		}

	}


}
