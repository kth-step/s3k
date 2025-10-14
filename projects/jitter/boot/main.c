#include "s3k.h"

#include <stdio.h>

// External function to initialize a process's memory and program counter.
// - mon_idx: Monitor index for the process.
// - ram_idx: Memory region index.
// - ram_base: Base address of the memory region.
// - ram_size: Size of the memory region.
extern bool proc_init(s3k_word_t mon_idx, int ram_idx, s3k_word_t ram_base, s3k_word_t ram_size);

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

extern char __uart_base[]; // UART base address, provided by the linker.

// Monitor indices for different process roles.
#define MON_MEASURER 4
#define MON_DUMMY 8
#define MON_INTERFERER1 12
#define MON_INTERFERER2 16

// Time-slice indices for each hart (hardware thread).
#define TSL_HART0 0
#define TSL_HART1 16

// Memory region indices.
#define MEM_DRAM 0
#define MEM_SPM 32

int main(void)
{
#ifdef SPM
	// Initialize MEASURER process in SPM memory region.
	proc_init(MON_MEASURER, MEM_SPM, 0x10010000, 0x10000);
#else
	// Initialize MEASURER process in DRAM memory region.
	proc_init(MON_MEASURER, MEM_DRAM, 0x80010000, 0x10000);
#endif

	// Initialize INTERFERER1 process in DRAM memory region.
	proc_init(MON_INTERFERER1, MEM_DRAM, 0x80040000, 0x20000);

	// Derive time-slices for MEASURER and INTERFERER1 on hart 0.
	s3k_mon_tsl_derive(MON_MEASURER, TSL_HART0, 2, true, 8);
	s3k_mon_tsl_derive(MON_INTERFERER1, TSL_HART0, 2, true, 16);

#ifdef DUAL
	// If DUAL is defined, initialize and schedule DUMMY and INTERFERER2 on hart 1.
	proc_init(MON_DUMMY, MEM_DRAM, 0x80020000, 0x20000);
	proc_init(MON_INTERFERER2, MEM_DRAM, 0x80060000, 0x20000);
	s3k_mon_tsl_derive(MON_DUMMY, TSL_HART1, 2, true, 8);
	s3k_mon_tsl_derive(MON_INTERFERER2, TSL_HART1, 2, true, 16);
#endif

	// Resume MEASURER and INTERFERER1 processes.
	s3k_mon_resume(MON_MEASURER);
	s3k_mon_resume(MON_INTERFERER1);
#ifdef DUAL
	// Resume DUMMY and INTERFERER2 if DUAL is defined.
	s3k_mon_resume(MON_DUMMY);
	s3k_mon_resume(MON_INTERFERER2);
#endif

	// Set the time-slice for hart 0 and synchronize to the next scheduling cycle.
	s3k_tsl_set(TSL_HART0, false);
	s3k_sleep_until(0); // Sleep until the next scheduling cycle.
}
