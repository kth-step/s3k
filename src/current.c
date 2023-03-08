#include "current.h"

register struct proc *current asm("tp");

void current_set(struct proc *proc)
{
	current = proc;
}

struct proc *current_get(void)
{
	return current;
}
