#include "ttas.h"

#include "preempt.h"

void ttas_init(ttas_t *ttas)
{
	ttas->lock = 0;
}

bool ttas_acquire(ttas_t *ttas, bool preemptable)
{
	while (__atomic_exchange_n(&ttas->lock, 1, __ATOMIC_ACQUIRE)) {
		if (preemptable && preempt()) {
			return false;
		}
	}
	return true;
}

void ttas_release(ttas_t *ttas)
{
	__atomic_store_n(&ttas->lock, 0, __ATOMIC_RELEASE);
}
