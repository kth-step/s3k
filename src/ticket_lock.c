#include "ticket_lock.h"

#include <stdint.h>

void tl_acq(struct ticket_lock *lock)
{
	// Increment next ticket number and return the previous value
	int ticket
	    = __atomic_fetch_add(&lock->next_ticket, 1, __ATOMIC_ACQUIRE);

	// Wait until our ticket number is being served
	while (lock->serving_ticket != ticket) {
		// Wait until our ticket number is being served
	}
	// Ensure visibility of memory operations
	__atomic_thread_fence(__ATOMIC_ACQUIRE);
}

void tl_rel(struct ticket_lock *lock)
{
	// Atomically increment the serving ticket number
	__atomic_fetch_add(&lock->serving_ticket, 1, __ATOMIC_RELEASE);
}
