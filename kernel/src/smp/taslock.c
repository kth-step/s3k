#include "smp/taslock.h"

#include "kernel.h"

#include <stdbool.h>

void taslock_init(taslock_t *l)
{
	l->lock = 0;
}

bool taslock_acquire(taslock_t *l)
{
	while (true) {
		while (__atomic_load_n(&l->lock, __ATOMIC_ACQUIRE)) {
			if (kernel_preempt())
				return false;
		}
		if (taslock_try_acquire(l))
			return true;
	}
}

bool taslock_try_acquire(taslock_t *l)
{
	return !__atomic_fetch_or(&l->lock, 1, __ATOMIC_ACQUIRE);
}

void taslock_release(taslock_t *l)
{
	__atomic_store_n(&l->lock, 0, __ATOMIC_RELEASE);
}
