#ifndef __PLAT_CONF_H__
#define __PLAT_CONF_H__

// Min and max usable hart ID.
#define S3K_MIN_HART 0
#define S3K_MAX_HART 3

// Total number of usable harts.
#define S3K_HART_CNT (S3K_MAX_HART - S3K_MIN_HART + 1ul)

// Number of PMP slots.
#define S3K_PMP_CNT 8

// RTC ticks per second
#define S3K_RTC_HZ 1000000ull

/// Stack size of 1024 KiB
#define S3K_LOG_STACK_SIZE 10

#define INIT_CAPS                                                     \
	{                                                             \
		[0] = cap_mk_pmp(0x20005fff, MEM_RWX),                \
		[1] = cap_mk_memory(0x80020000, 0x80100000, MEM_RWX), \
		[2] = cap_mk_memory(0x10000000, 0x10001000, MEM_RW),  \
		[3] = cap_mk_memory(0x200b000, 0x200c000, MEM_R),     \
		[4] = cap_mk_time(0, 0, S3K_SLOT_CNT),                \
		[5] = cap_mk_time(1, 0, S3K_SLOT_CNT),                \
		[6] = cap_mk_time(2, 0, S3K_SLOT_CNT),                \
		[7] = cap_mk_time(3, 0, S3K_SLOT_CNT),                \
		[8] = cap_mk_monitor(0, S3K_PROC_CNT),                \
		[9] = cap_mk_channel(0, S3K_CHAN_CNT),                \
	}

#endif /* __PLAT_CONF_H__ */
