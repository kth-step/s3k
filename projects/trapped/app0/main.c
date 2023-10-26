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

void setup_uart(s3k_cidx_t uart_idx);
char trap_stack[1024];
void trap_handler(void) __attribute__((interrupt("machine")));

void trap_handler(void)
{
	// We enter here on illegal instructions, for example writing to
	// protected area (UART).

	// On an exception we do
	// - tf.epc = tf.pc (save program counter)
	// - tf.pc = tf.tpc (load trap handler address)
	// - tf.esp = tf.sp (save stack pointer)
	// - tf.sp = tf.tsp (load trap stack pointer)
	// - tf.ecause = mcause (see RISC-V privileged spec)
	// - tf.eval = mval (see RISC-V privileged spec)
	// tf is the trap frame, all registers of our process
	uint64_t epc = s3k_reg_read(S3K_REG_EPC);
	uint64_t esp = s3k_reg_read(S3K_REG_ESP);
	uint64_t ecause = s3k_reg_read(S3K_REG_ECAUSE);
	uint64_t eval = s3k_reg_read(S3K_REG_EVAL);

	setup_uart(16);
	alt_puts("uart is now setup!");
	alt_printf(
	    "error info:\n- epc: 0x%x\n- esp: 0x%x\n- ecause: 0x%x\n- eval: 0x%x\n",
	    epc, esp, ecause, eval);
	alt_printf("restoring pc and sp\n\n");
	// __attribute__((interrupt("machine"))) replaces `ret` with an `mret`.
	// When mret is executed in user-mode, the kernel catches it setting the
	// following:
	// - tf.pc = tf.epc
	// - tf.sp = tf.esp
	// Restoring pc and sp to the previous values, unless epc and esp was
	// overwritten.
}

void setup_trap(void)
{
	// Sets the trap handler
	s3k_reg_write(S3K_REG_TPC, (uint64_t)trap_handler);
	// Set the trap stack
	s3k_reg_write(S3K_REG_TSP, (uint64_t)trap_stack + 1024);
}

void setup_uart(s3k_cidx_t uart_idx)
{
	uint64_t uart_addr = s3k_napot_encode(0x10000000, 0x8);
	// Derive a PMP capability for accessing UART
	s3k_cap_derive(UART_MEM, uart_idx, s3k_mk_pmp(uart_addr, S3K_MEM_RW));
	// Load the derive PMP capability to PMP configuration
	s3k_pmp_load(uart_idx, 1);
	// Synchronize PMP unit (hardware) with PMP configuration
	// false => not full synchronization.
	s3k_sync_mem();
}

int main(void)
{
	setup_trap();

	// Write hello world.
	// But we have not created a PMP capability!
	alt_puts("hello, world from app0");

	// BYE!
	alt_puts("bye from app0");
}
