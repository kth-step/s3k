#include <interrupt.h>
#include <proc.h>
#include <sched.h>

proc_t *interrupt_handler(proc_t *proc, uint64_t mcause, uint64_t mtval)
{
	proc_release(proc);
	return sched();
}
