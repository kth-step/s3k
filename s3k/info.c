// See LICENSE file for copyright and license details.
#ifndef NDEBUG

#include "info.h"

#include "kprint.h"
#include "platform.h"

void print_info(void)
{
        kprintf("[===========================================]\n");
        kprintf("[S3K Information\n");
        kprintf("[-------------------------------------------]\n");
        kprintf("[Platform:                        %10s]\n", PLATFORM_NAME);
        kprintf("[Hart count:                      %10ld]\n", (unsigned long)N_CORES);
        kprintf("[Min hartid:                      %10lu]\n", (unsigned long)MIN_HARTID);
        kprintf("[Max hartid:                      %10lu]\n", (unsigned long)MAX_HARTID);
        kprintf("[Process count:                   %10lu]\n", (unsigned long)N_PROC);
        kprintf("[Major frame length (ticks):      %10lu]\n", (unsigned long)TICKS * N_QUANTUM);
        kprintf("[Minor frame granularity (ticks): %10lu]\n", (unsigned long)TICKS);
        kprintf("[Max quanta per major frame:      %10lu]\n", (unsigned long)N_QUANTUM);
        kprintf("[Slack/scheduler ticks:           %10lu]\n", (unsigned long)SCHEDULER_TICKS);
        kprintf("[Ticks per second:                %10lu]\n", (unsigned long)TICKS_PER_SECOND);
        kprintf("[===========================================]\n");
}
#endif
