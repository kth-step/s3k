#include "altc/altio.h"
#include "s3k/s3k.h"

#include "ff.h"

#include "../../tutorial-commons/utils.h"

int main(void)
{
	setup_uart_and_virtio();

	alt_printf("hello, world\n");

	FATFS FatFs;					/* FatFs work area needed for each volume */
	f_mount(&FatFs, "", 0);			/* Give a work area to the default drive */
	alt_printf("File system mounted\n");

	UINT bw;
	FRESULT fr;
	FIL Fil;									/* File object needed for each open file */
	fr = f_open(&Fil, "newfile.txt", FA_WRITE | FA_CREATE_ALWAYS);	/* Create a file */
	if (fr == FR_OK) {
		f_write(&Fil, "It works!\r\n", 11, &bw);	/* Write data to the file */
		fr = f_close(&Fil);							/* Close the file */
		if (fr == FR_OK && bw == 11) {
			alt_printf("File saved\n");
		}
	} else{
		alt_printf("File not opened\n");
	}

	char buffer[1024];
	fr = f_open(&Fil, "test.txt", FA_READ);
	if (fr == FR_OK) {
		f_read(&Fil, buffer, 1023, &bw);	/*Read data from the file */
		fr = f_close(&Fil);							/* Close the file */
		if (fr == FR_OK) {
			buffer[bw] = '\0';
			alt_puts(buffer);
		}
	} else{
		alt_puts("File not opened\n");
	}
}
