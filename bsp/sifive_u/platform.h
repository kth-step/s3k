#pragma once

/* Number of cores */
#define N_CORES 5
/* Number of PMP registers in hardware */
#define N_PMP 8
/* Ticks per second */
#define TICKS_PER_SECOND 1000000UL
/* Minimum hartid usable by the kernel */
#define MIN_HARTID 1
/* Maximum hartid */
#define MAX_HARTID 4

/* Clint memory location */
#define CLINT 0x2000000ull

/* Stack size. */
/* log_2 of stack size. */
#define LOG_STACK_SIZE 12
#define STACK_SIZE (1UL << LOG_STACK_SIZE)

#define PLATFORM_NAME "sifive_u"
