#include <exception.h>
#include <interrupt.h>
#include <sched.h>
#include <syscall.h>
#include <trap.h>

proc_t *trap_handler(proc_t *proc, val_t mcause, val_t mtval)
{
	if (mcause == 8) {
		proc_t *next = syscall_handler(proc);
		if (next == proc)
			trap_return(proc);
		proc_release(proc);
		if (next)
			return next;
		return sched();
	} else if ((int64_t)mcause < 0) {
		return interrupt_handler(proc, mcause, mtval);
	} else {
		trap_return(exception_handler(proc, mcause, mtval));
	}
}
