// TODO: Replace with proper MCS Lock
#include "mcslock.h"

#include "preempt.h"

#include <stddef.h>

static void _release(qnode_t *me)
{
	qnode_t *next, *prev;

	prev = __atomic_exchange_n(&me->prev, NULL, __ATOMIC_RELAXED);

	do {
		next = me->next;
	} while (__atomic_compare_exchange(&next->prev, &me, &prev, false,
					   __ATOMIC_RELEASE, __ATOMIC_RELAXED));
	if (prev != NULL)
		prev->next = next;
}

static bool _acquire(qnode_t *tail, qnode_t *me, bool preemptive)
{
	me->next = tail;
	me->prev = __atomic_exchange_n(&tail->prev, me, __ATOMIC_ACQUIRE);

	// If there is a predecessor, set its next ptr to me.
	if (me->prev)
		me->prev->next = me;

	// Wait while me->prev != NULL
	while (__atomic_load_n(&me->prev, __ATOMIC_ACQUIRE)) {
		if (preemptive && preempt()) {
			_release(me);
			return false;
		}
	}
	return true;
}

void mcslock_init(mcslock_t *lock, qnode_t *nodes)
{
	lock->tail.prev = lock->tail.next = NULL;
	lock->nodes = nodes;
}

void mcslock_acquire(mcslock_t *lock, uint64_t me)
{
	_acquire(&lock->tail, &lock->nodes[me], false);
}

bool mcslock_try_acquire(mcslock_t *lock, uint64_t me)
{
	return _acquire(&lock->tail, &lock->nodes[me], true);
}

void mcslock_release(mcslock_t *lock, uint64_t me)
{
	_release(&lock->nodes[me]);
}
