/**
 * A counting semaphore based on a ticket lock.
 */
#include "smp/semaphore.h"

void semaphore_init(semaphore_t *sem, uint64_t tickets)
{
	// Initial number of served tickets.
	sem->released = tickets;
	// Initiallly, 0 tickets are released.
	sem->acquired = 0;
}

void semaphore_acquire_n(semaphore_t *sem, uint64_t n)
{
	uint64_t my_ticket;
	// Acquire n tickets.
	my_ticket = __atomic_add_fetch(&sem->acquired, n, __ATOMIC_RELAXED);
	// Wait until the tickets are released.
	while (my_ticket > __atomic_load_n(&sem->released, __ATOMIC_ACQUIRE))
		;
}

void semaphore_release_n(semaphore_t *sem, uint64_t n)
{
	// Release n tickets.
	__atomic_fetch_add(&sem->released, n, __ATOMIC_RELEASE);
}

void semaphore_acquire(semaphore_t *sem)
{
	semaphore_acquire_n(sem, 1);
}

void semaphore_release(semaphore_t *sem)
{
	semaphore_release_n(sem, 1);
}
