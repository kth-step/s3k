#include "kern/interrupt.h"

#include "kern/proc.h"
#include "kern/sched.h"

void interrupt_handler(uint64_t mcause, uint64_t mtval)
{
	proc_release(current);
	sched();
}
