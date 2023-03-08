#include "current.h"

register struct proc *current asm("tp");

void current_set(struct proc *proc)
{
	current = proc;
}

void current_get(struct proc *proc)
{
	current = proc;
}
