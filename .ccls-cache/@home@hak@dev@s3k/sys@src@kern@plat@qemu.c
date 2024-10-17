#include "drivers/uart/ns16550a.h"
#include "kern/cap.h"

const cap_t init_caps[] = {
    CAP_PMP(0X20005fff, MEM_RWX),
    CAP_MEMORY(0, 0x80020000, 0x88000000, MEM_RWX),
    CAP_MEMORY(0, 0x10000000, 0x10001000, MEM_RW),
    CAP_MEMORY(0, 0x200b000, 0x200c000, MEM_R),
    CAP_TIME(0, S3K_SLOT_CNT),
    CAP_MONITOR(0, S3K_PROC_CNT),
    CAP_CHANNEL(0, S3K_CHAN_CNT),
    0,
};

char kernel_stack[256];

static uart_ns16550a_t uart = UART_NS16550A(0x10000000ULL);

FILE *const _altin = (FILE *)&uart;
FILE *const _altout = (FILE *)&uart;
