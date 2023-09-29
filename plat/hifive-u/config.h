#ifndef __PLATFORM_H__
#define __PLATFORM_H__

// Min and max usable hart ID.
#define MIN_HARTID 1
#define MAX_HARTID 4

// Total number of usable harts.
#define N_HART 4

// Number of PMP slots.
#define N_PMP 8

// RTC ticks per second
#define TICKS_PER_SECOND 1000000ull

/// Stack size of 256 B
#define LOG_STACK_SIZE 8

// Initial capabilities.
#define INIT_CAPS                                                           \
	{                                                                   \
		CAP_PMP(0x20005fff, CAP_RWX),                               \
		    CAP_MEMORY(0x80020, 0x80, CAP_RWX),                     \
		    CAP_MEMORY(0x10010, 0x1, CAP_RW),                       \
		    CAP_MEMORY(0x200b, 0x1, CAP_R), CAP_TIME(1, 0, N_SLOT), \
		    CAP_TIME(2, 0, N_SLOT), CAP_TIME(3, 0, N_SLOT),         \
		    CAP_TIME(4, 0, N_SLOT), CAP_MONITOR(0, N_PROC),         \
		    CAP_CHANNEL(0, N_CHAN)                                  \
	}

#endif /* __PLATFORM_H__ */
