#include "altc/altio.h"
#include "s3k/s3k.h"

#include "../../tutorial-commons/utils.h"



int main(void)
{
	// Setup UART access
	setup_uart();

	// Setup app1 capabilities and PC
	setup_app_1();

	// Write hello world.
	alt_printf("hello, world from app0\n");

	// Setup scehduling
	setup_scheduling(ROUND_ROBIN);

	// Start app1
	log_sys("10", s3k_mon_resume(MONITOR, APP1_PID)); 

	while (1)
		alt_puts("0");


	// BYE!
	alt_puts("bye from app0");
}
