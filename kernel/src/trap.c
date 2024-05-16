#include <exception.h>
#include <interrupt.h>
#include <sched.h>
#include <syscall.h>
#include <trap.h>

proc_t *trap_handler(proc_t *proc, uint64_t mcause, uint64_t mtval)
{
	if (mcause == 8) {
		proc_t *next = syscall_handler(proc);
		if (next != proc)
			proc_release(proc);
		if (next)
			return next;
		return sched();
	} else if ((int64_t)mcause < 0) {
		return interrupt_handler(proc, mcause, mtval);
	} else {
		return exception_handler(proc, mcause, mtval);
	}
}
