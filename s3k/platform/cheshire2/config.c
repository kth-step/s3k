#include "csr.h"
#include "ipc.h"
#include "lock.h"
#include "mem.h"
#include "mon.h"
#include "pmp.h"
#include "proc.h"
#include "sched.h"
#include "syscall.h"
#include "tsl.h"

// Define memory regions and permissions as constants
#define RAM_PERM MEM_PERM_RWX
#define RAM_BASE 0x80000000
#define RAM_SIZE 0x10000000

// Define memory regions and permissions as constants
#define SPM_PERM MEM_PERM_RWX
#define SPM_BASE 0x10010000
#define SPM_SIZE 0x10000

#define UART_PERM MEM_PERM_RW
#define UART_BASE 0x03002000
#define UART_SIZE 0x20

_Static_assert(_NUM_HARTS == 2);
_Static_assert(_NUM_MEMORY_CAPS == 3);

void kernel_init(void)
{
	mem_t init_mem[_NUM_MEMORY_CAPS] = {
		{.rwx = RAM_PERM,  .base = RAM_BASE,  .size = RAM_SIZE },
		{.rwx = UART_PERM, .base = UART_BASE, .size = UART_SIZE},
		{.rwx = SPM_PERM,  .base = SPM_BASE,  .size = SPM_SIZE },
	};

	mem_init(init_mem);
	tsl_init();
	mon_init();
	ipc_init();
	sched_init();
	lock_init();
	proc_init(RAM_BASE);

	mem_pmp_set((pid_t)1, (index_t)0, (pmp_slot_t)1, RAM_PERM, pmp_napot_encode(RAM_BASE, RAM_SIZE));
	mem_pmp_set((pid_t)1, (index_t)MAX_MEMORY_FUEL, (pmp_slot_t)2, UART_PERM,
		    pmp_napot_encode(UART_BASE, UART_SIZE));
	mem_pmp_set((pid_t)1, (index_t)2 * MAX_MEMORY_FUEL, (pmp_slot_t)3, SPM_PERM,
		    pmp_napot_encode(SPM_BASE, SPM_SIZE));
}

void temporal_fence(void)
{
#if CSPAD != 0
	__asm__ volatile(".word 0xb");
#endif
}
