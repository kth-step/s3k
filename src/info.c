// See LICENSE file for copyright and license details.
#include "info.h"

#include "kprint.h"
#include "platform.h"

void print_info(void)
{
        kprintf("===================================================\n");
        kprintf("S3K Information\n");
        kprintf("---------------------------------------------------\n");
        kprintf("Platform:                     %s\n", PLATFORM_NAME);
        kprintf("Hart count:                   %d\n", N_CORES);
        kprintf("Usable harts:                 %d-%d\n", MIN_HARTID, MAX_HARTID);
        kprintf("Process count:                %d\n", N_PROC);
        kprintf("Major frame length:           %d ticks\n", TICKS * N_QUANTUM);
        kprintf("Minor frame granularity:      %d ticks\n", TICKS);
        kprintf("Max quanta per major frame:   %d\n", N_QUANTUM);
        kprintf("Slack/scheduler ticks:        %d\n", SCHEDULER_TICKS);
        kprintf("Ticks per second:             %d\n", TICKS_PER_SECOND);
        kprintf("===================================================\n");
}
