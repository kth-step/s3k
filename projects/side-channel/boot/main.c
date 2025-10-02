#include "s3k.h"
#include <stdio.h>

// External function to initialize a process's memory and pc.
extern bool proc_init(s3k_word_t mon_idx, int ram_idx, s3k_word_t ram_base, s3k_word_t ram_size);

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

// Monitor, memory, and time slice indices
#define MON_SPY    4
#define MON_DUMMY  8
#define MON_TROJAN 12

#define TSL_HART0  0
#define TSL_HART1  16

#define MEM_DRAM   0
#define MEM_SPM    32

int main(void)
{
#ifdef SPM
    // Initialize SPY process in SPM memory region
    proc_init(MON_SPY, MEM_SPM, 0x10010000, 0x10000);
#else
    // Initialize SPY process in DRAM memory region
    proc_init(MON_SPY, MEM_DRAM, 0x80010000, 0x10000);
#endif
    // Initialize TROJAN process in DRAM memory region
    proc_init(MON_TROJAN, MEM_DRAM, 0x80100000, 0x100000);

    s3k_sleep_until(0); // Synchronize to the next scheduling cycle

    // Derive time-slice for SPY and TROJAN on hart 0
    s3k_mon_tsl_derive(MON_SPY, TSL_HART0, 8, true, 8);
    s3k_mon_tsl_derive(MON_TROJAN, TSL_HART0, 1, true, 23);

#ifdef DUAL
    // If DUAL is defined, initialize DUMMY process on hart 1
    // Dummy is scheduled concurrently with the spy.
    proc_init(MON_DUMMY, MEM_DRAM, 0x80020000, 0x20000);
    s3k_mon_tsl_derive(MON_DUMMY, TSL_HART1, 1, true, 8);
    s3k_mon_resume(MON_DUMMY); // Resume DUMMY process
#endif

    s3k_sleep_until(0); // Synchronize to the next scheduling cycle
    printf("signal,jitter,cache,syscall\n");
    // Resume SPY and TROJAN processes
    s3k_mon_resume(MON_SPY);
    s3k_mon_resume(MON_TROJAN);

    // Set the time-slice for hart 0 and synchronize again
    s3k_tsl_set(TSL_HART0, false);
    s3k_sleep_until(0); // Synchronize to the next cycle
}
