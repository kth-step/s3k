#pragma once

#include "plat/config.h"

// Number of user processes
#define S3K_PROC_CNT 6

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
#define INSTRUMENT_WCRT
#define VERBOSITY 2

#define MEASUREMENTS 100000

//#define SCENARIO_CAP_OPS
//#define SCENARIO_PMP
//#define SCENARIO_MONITOR
//#define SCENARIO_SOCKET
#define SCENARIO_ALL
