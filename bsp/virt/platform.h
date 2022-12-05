#pragma once

/* Platform name */
#define PLATFORM_NAME "virt"

/* Cores count */
#define N_CORES 5

/* PMP count */
#define N_PMP 8

/* Ticks per second */
#define TICKS_PER_SECOND 10000000UL

/* Min and max hartid usable by the kernel */
#define MIN_HARTID 1
#define MAX_HARTID 4

/* Number of usable harts */
#define N_HARTS (MAX_HARTID - MIN_HARTID + 1)

/* Clint memory location */
#define CLINT 0x2000000ull

/* Stack size. */
/* log_2 of stack size. */
#define LOG_STACK_SIZE 10
#define STACK_SIZE (1UL << LOG_STACK_SIZE)
