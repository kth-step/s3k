// TODO: Replace with proper MCS Lock
#include "smp/mcslock.h"

#include "kassert.h"
#include "kernel.h"

#include <stddef.h>

static void _release(qnode_t *const node)
{
	qnode_t *expected, *next, *prev;
	prev = __atomic_exchange_n(&node->prev, NULL, __ATOMIC_RELAXED);
	do {
		expected = node;
		next = node->next;
	} while (!__atomic_compare_exchange_n(&next->prev, &expected, prev,
					      false, __ATOMIC_RELEASE,
					      __ATOMIC_RELAXED));
	if (prev)
		prev->next = next;
}

static bool _acquire(mcslock_t *lock, qnode_t *const node, bool preemptive)
{
	node->next = &lock->tail;
	node->prev
	    = __atomic_exchange_n(&lock->tail.prev, node, __ATOMIC_ACQUIRE);
	if (node->prev == NULL)
		return true;
	node->prev->next = node;
	while (__atomic_load_n(&node->prev, __ATOMIC_ACQUIRE)) {
		if (preemptive && kernel_preempt()) {
			_release(node);
			return false;
		}
	}
	return true;
}

void mcslock_init(mcslock_t *lock)
{
	// lock->tail.prev = lock->tail.next = NULL;
	lock->tail.prev = lock->tail.next = NULL;
}

void mcslock_acquire(mcslock_t *lock, qnode_t *node)
{
	_acquire(lock, node, false);
}

bool mcslock_try_acquire(mcslock_t *lock, qnode_t *node)
{
	return _acquire(lock, node, true);
}

void mcslock_release(mcslock_t *lock, qnode_t *node)
{
	_release(node);
}
