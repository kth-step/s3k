#include <interrupt.h>
#include <proc.h>
#include <sched.h>

proc_t *interrupt_handler(proc_t *proc, val_t mcause, val_t mtval)
{
	proc_release(proc);
	return sched();
}
