// Default kernel configuration
#ifndef __CONFIG_H__
#define __CONFIG_H__

// Number of user processes
#define N_PROC 8

// Number of capabilities per process.
#define N_CAP 32

// Number of IPC channels.
#define N_CHAN 16

// Number of slots per period
#define N_SLOT 64ull

// Length of slots in ticks.
#define SLOT_LENGTH (TICKS_PER_SECOND / N_SLOT / 1ull)

// Scheduler time
#define SCHEDULER_TIME 10000

// If debugging, uncomment
// #define NDEBUG

// If instrumenting
#define INSTRUMENTATION

#endif /* __CONFIG_H__ */
