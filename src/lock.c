#include "lock.h"

void tl_lock(struct ticket_lock *l)
{
	uint64_t ticket
	    = __atomic_fetch_add(&l->next_tick, 1, __ATOMIC_ACQUIRE);
	while (l->serving != ticket)
		;
}

void tl_unlock(struct ticket_lock *l)
{
	__atomic_thread_fence(__ATOMIC_RELEASE);
	l->serving++;
}
