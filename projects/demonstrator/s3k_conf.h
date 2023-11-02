#pragma once

#define PLATFORM_HIFIVE_UNLEASHED

#include "plat/config.h"

// Number of user processes
#define S3K_PROC_CNT 6

// Number of capabilities per process.
#define S3K_CAP_CNT 32

// Number of IPC channels.
#define S3K_CHAN_CNT 4

// Number of slots per period
#define S3K_SLOT_CNT 32ull

// Length of slots in ticks.
#define S3K_SLOT_LEN (S3K_RTC_HZ / S3K_SLOT_CNT / 10ull)

// Scheduler time
#define S3K_SCHED_TIME 1

// If debugging, comment
//#define NDEBUG
#define INSTRUMENT
