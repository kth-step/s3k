#pragma once

#define PLATFORM_VIRT
#include "plat/config.h"

// Number of user processes
#define S3K_PROC_CNT 2

// Number of capabilities per process.
#define S3K_CAP_CNT 32

// Number of IPC channels.
#define S3K_CHAN_CNT 2

// Number of slots per period
#define S3K_SLOT_CNT 32ull

// Length of slots in ticks.
#define S3K_SLOT_LEN (S3K_RTC_HZ / S3K_SLOT_CNT / 100ull)

// Scheduler time
#define S3K_SCHED_TIME (S3K_SLOT_LEN / 10)

// If debugging, comment
// #define NDEBUG
