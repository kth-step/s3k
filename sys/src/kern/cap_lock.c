#include "kern/cap_lock.h"

#include "kern/csr.h"
#include "smp/taslock.h"

static taslock_t lock;

void cap_lock_init(void)
{
	taslock_init(&lock);
}

bool cap_lock_acquire(void)
{
	return taslock_acquire(&lock);
}

void cap_lock_release(void)
{
	taslock_release(&lock);
}
