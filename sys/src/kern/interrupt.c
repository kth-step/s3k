#include "kern/interrupt.h"

#include "kern/proc.h"
#include "kern/sched.h"

Proc *InterruptHandler(Proc *proc, Word mcause, Word mtval)
{
	ProcRelease(proc);
	return SchedNext();
}
