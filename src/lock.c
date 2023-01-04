/* See LICENSE file for copyright and license details. */
#include "lock.h"

void spinlock_lock(struct spinlock *l)
{
	while (!spinlock_trylock(l))
		;
}

bool spinlock_trylock(struct spinlock *l)
{
	return !__atomic_fetch_or(&l->lock, 1, __ATOMIC_ACQUIRE);
}

void spinlock_unlock(struct spinlock *l)
{
	__atomic_store_n(&l->lock, 0, __ATOMIC_RELEASE);
}

void ticklock_lock(struct ticklock *l)
{
	uint64_t ticket = __atomic_fetch_add(&l->next_tick, 1, __ATOMIC_ACQUIRE);
	while (__atomic_load_n(&l->serving, __ATOMIC_RELAXED) != ticket)
		;
}

void ticklock_unlock(struct ticklock *l)
{
	__atomic_store_n(&l->serving, l->serving + 1, __ATOMIC_RELEASE);
}
