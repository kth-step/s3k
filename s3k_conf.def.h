// Default kernel configuration
#ifndef __S3K_CONF_H__
#define __S3K_CONF_H__

// Number of user processes
#define S3K_PROC_CNT 8

// Number of capabilities per process.
#define S3K_CAP_CNT 32

// Number of IPC channels.
#define S3K_CHAN_CNT 16

// Number of slots per period
#define S3K_SLOT_CNT 64ull

// Length of slots in ticks.
#define S3K_SLOT_LEN (S3K_RTC_HZ / S3K_SLOT_CNT / 100ull)

// Scheduler time
#define S3K_SCHED_TIME (S3K_SLOT_LEN / 10)

//! You should probably not change these variables
// Min logarithmic size of a memory slice
#define S3K_MIN_BLOCK_SIZE 12
// Max logarithmic size of a memory slice
#define S3K_MAX_BLOCK_SIZE (S3K_MIN_BLOCK_SIZE + 15)

// If debugging, comment
// #define NDEBUG

#endif /* __S3K_CONF_H__ */
