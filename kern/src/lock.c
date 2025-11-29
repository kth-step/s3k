#include "preempt.h"
#include "ttas.h"

#ifdef SMP
static ttas_t ttas = {0};

void lock_init(void)
{
	ttas_init(&ttas);
}

bool lock_acquire(bool preemptable)
{
	return ttas_acquire(&ttas, preemptable);
}

void lock_release(void)
{
	ttas_release(&ttas);
}
#else

void lock_init(void)
{
}

bool lock_acquire(bool preemptable)
{
	return preemptable || !preempt();
}

void lock_release(void)
{
}
#endif
