#ifndef __PLATFORM_H__
#define __PLATFORM_H__


// Min and max usable hart ID.
#define MIN_HARTID 0
#define MAX_HARTID 3

// Total number of usable harts.
#define N_HART 4

// Number of PMP slots.
#define N_PMP 8

// RTC ticks per second
#define TICKS_PER_SECOND 1000000ull

/// Stack size of 1024 KiB
#define LOG_STACK_SIZE 10

// Initial capabilities.
#define INIT_CAPS                                                           \
	{                                                                   \
		cap_mk_pmp(0x20005fff, MEM_RWX),                               \
		    cap_mk_memory(0x80020, 0x80, MEM_RWX),                     \
		    cap_mk_memory(0x10000, 0x1, MEM_RW),                       \
		    cap_mk_memory(0x200b, 0x1, MEM_R), cap_mk_time(0, 0, N_SLOT), \
		    cap_mk_time(1, 0, N_SLOT), cap_mk_time(2, 0, N_SLOT),         \
		    cap_mk_time(3, 0, N_SLOT), cap_mk_monitor(0, N_PROC),         \
		    cap_mk_channel(0, N_CHAN)                                  \
	}

#endif /* __PLATFORM_H__ */
