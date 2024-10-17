/**
 * Platform configuration for QEMU virt
 */
#pragma once

#define UART_NS16550A
#define UART0_BASE_ADDR (0x10000000ull)

#define MTIME_BASE_ADDR 0x200bff8ull
#define MTIMECMP_BASE_ADDR 0x2004000ull

// Min and max usable hart ID.
#define S3K_MIN_HART 0
#define S3K_MAX_HART 0

// Total number of usable harts.
#define S3K_HART_CNT (S3K_MAX_HART - S3K_MIN_HART + 1ul)

// Number of PMP slots.
#define S3K_PMP_CNT 8

// RTC ticks per second
#define S3K_RTC_HZ 1000000ull

/// Stack size of 1024 KiB
#define S3K_LOG_STACK_SIZE 10
