// See LICENSE file for copyright and license details.
#pragma once

/* Number of processes. */
#define N_PROC 4

/* Number of capabilities per process */
#define N_CAPS 32

/* Number of time slices in a major frame. */
#define N_QUANTUM 64

/* Number of ipc ports */
#define N_PORTS 10

/* Number of ticks per quantum. */
/* TICKS_PER_SECOND defined in platform.h */
#define TICKS (TICKS_PER_SECOND / N_QUANTUM / 100)

/* Number of scheduler ticks. */
#define SCHEDULER_TICKS 2000

/* Uncomment to enable memory protection */
//#define MEMORY_PROTECTION
