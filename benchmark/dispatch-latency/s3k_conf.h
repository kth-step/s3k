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
#define INSTRUMENT_DISPATCH_LATENCY
#define VERBOSITY 2

// Selection of scenarios
#define SCENARIO_SOLO 0
#define SCENARIO_ACTIVE 1
#define SCENARIO_FLUSH 2
#define SCENARIO_ACTIVE_FLUSH 3
// Set SCENARIO to one of the above
#define SCENARIO SCENARIO_FLUSH
