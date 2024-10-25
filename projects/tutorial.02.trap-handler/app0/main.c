#include "altc/altio.h"
#include "s3k/s3k.h"

#include "../../tutorial-commons/utils.h"

char trap_stack[1024];
void trap_handler(void) __attribute__((interrupt("machine")));


void trap_handler(void)
{
	setup_uart();
	alt_puts("uart is now setup!");
	default_trap_handler();
}

int main(void)
{
	// Setup trap handler
	setup_trap(trap_handler, trap_stack, 1024);

	// Write hello world.
	alt_printf("Hello, world\n");
}
