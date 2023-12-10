#pragma once

#include "plat/config.h"

// Number of user processes
#define S3K_PROC_CNT 4

// Number of capabilities per process.
#define S3K_CAP_CNT 32

// Number of IPC channels.
#define S3K_CHAN_CNT 16

// Number of slots per period
#define S3K_SLOT_CNT 32ull

// Length of slots in ticks.
#define S3K_SLOT_LEN (S3K_RTC_HZ / S3K_SLOT_CNT / 100)

// Scheduler time
#define S3K_SCHED_TIME 8
//#define NPREMPT

// If debugging, comment
#define NDEBUG
#define INSTRUMENT_CYCLE
#define VERBOSITY 2

//#define SCENARIO_SOLO
#define SCENARIO_ACTIVE
//#define SCENARIO_FLUSH
//#define SCENARIO_ACTIVE_FLUSH
