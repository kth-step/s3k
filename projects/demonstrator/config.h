#pragma once

/* Boot process */
#define BOOT_PID 0
#define BOOT_MEM ((char *)0x80010000ull)
#define BOOT_LEN 0x10000

/* Monitor process */
#define MONITOR_PID 1
#define MONITOR_MEM ((char *)0x80020000ull)
#define MONITOR_MEM_LEN (0x8000ull)

/* Crypto process */
#define CRYPTO_PID 2
#define CRYPTO_MEM ((char *)0x80028000ull)
#define CRYPTO_MEM_LEN (0x8000ull)

/* UART process */
#define UART_PID 3
#define UART_TIME 10 /* number of time slots for uart */
#define UART_MEM ((char *)0x80030000ull)
#define UART_MEM_LEN (0x8000ull)

/* Shared memory between monitor and uart */
#define SHARED0_MEM ((char *)0x80038000ull)
#define SHARED0_MEM_LEN (0x4000ull)
/* Shared memory between monitor and crypto */
#define SHARED1_MEM ((char *)0x8003C000ull)
#define SHARED1_MEM_LEN (0x4000ull)

/* Application 0 memory area */
#define APP0_PID 4
#define APP0_MEM ((char *)0x80040000ull)
#define APP0_MEM_LEN (0x8000ull)

/* Application 1 memory area */
#define APP1_PID 5
#define APP1_MEM ((char *)0x80048000ull)
#define APP1_MEM_LEN (0x8000ull)
