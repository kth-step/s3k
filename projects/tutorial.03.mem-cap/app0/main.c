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

/* initial configuration for memory is in */
/* /plat/virt/config.h */

#define TAG_BLOCK_TO_ADDR(tag, block) ( \
					(((uint64_t) tag) << S3K_MAX_BLOCK_SIZE) + \
					(((uint64_t) block) << S3K_MIN_BLOCK_SIZE) \
					)

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

void s3k_print_cap(s3k_cap_t *cap) {
	if (!cap)
		alt_printf("Capability is NULL\n");
	switch ((*cap).type) {
	case S3K_CAPTY_NONE:
		alt_printf("No Capability\n");
		break;
	case S3K_CAPTY_TIME:
		alt_printf("Time hart:%X bgn:%X mrk:%X end:%Z\n",
				   (*cap).time.hart, (*cap).time.bgn, (*cap).time.mrk, (*cap).time.end);
		break;
	case S3K_CAPTY_MEMORY:
		alt_printf("Memory rwx:%X lock:%X bgn:%X mrk:%X end:%X\n",
				   (*cap).mem.rwx, (*cap).mem.lck,
				   TAG_BLOCK_TO_ADDR((*cap).mem.tag, (*cap).mem.bgn),
				   TAG_BLOCK_TO_ADDR((*cap).mem.tag, (*cap).mem.mrk),
				   TAG_BLOCK_TO_ADDR((*cap).mem.tag, (*cap).mem.end)
				   );
		break;
	case S3K_CAPTY_PMP:
		alt_printf("PMP rwx:%X used:%X index:%X address:%Z\n",
				   (*cap).pmp.rwx, (*cap).pmp.used, (*cap).pmp.slot, (*cap).pmp.addr);
		break;
	case S3K_CAPTY_MONITOR:
		alt_printf("Monitor  bgn:%X mrk:%X end:%X\n",
				    (*cap).mon.bgn, (*cap).mon.mrk, (*cap).mon.end);
		break;
	case S3K_CAPTY_CHANNEL:
		alt_printf("Channel  bgn:%X mrk:%X end:%X\n",
				    (*cap).chan.bgn, (*cap).chan.mrk, (*cap).chan.end);
		break;
	case S3K_CAPTY_SOCKET:
		alt_printf("Socket  mode:%X perm:%X channel:%X tag:%X\n",
				    (*cap).sock.mode, (*cap).sock.perm, (*cap).sock.chan, (*cap).sock.tag);
		break;
	}
}

int main(void)
{
	// Setup UART access
	setup_uart(10);

	// Write hello world.
	alt_puts("Hello, world");

	s3k_cap_t cap;
	while (s3k_cap_read(RAM_MEM, &cap));
	s3k_print_cap(&cap);

	alt_printf("cap derivation result %X\n",
			   s3k_cap_derive(1, 17, s3k_mk_memory(0x80020000, 0x80030000, S3K_MEM_RWX)));
	alt_printf("cap derivation result %X\n",
			   s3k_cap_derive(1, 18, s3k_mk_memory(0x80020000, 0x80030000, S3K_MEM_RWX)));
	while (s3k_cap_read(RAM_MEM, &cap));
	s3k_print_cap(&cap);
	while (s3k_cap_read(17, &cap));
	s3k_print_cap(&cap);

	uint64_t pmp_addr = s3k_napot_encode(0x80020000, 0x10000);
	alt_printf("cap derivation result %X\n",
			   s3k_cap_derive(17, 18, s3k_mk_pmp(pmp_addr, S3K_MEM_RW)));
	while (s3k_cap_read(17, &cap));
	s3k_print_cap(&cap);

	alt_printf("Loading the PMP\n");
	while (s3k_pmp_load(18, 2));
	alt_printf("Sync status\n");
	s3k_sync_mem();

	alt_printf("Accessing the new memory\n");
	*((uint64_t *)(0x80020000)) = 10;
	alt_printf("Successfully wrote in random memory 0x%X\n", *((uint64_t *)(0x80020000)));
}
