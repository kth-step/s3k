#include "cap_lock.h"

#include "csr.h"
#include "mcslock.h"

#ifdef SMP
static mcslock_t lock;
static qnode_t nodes[S3K_MAX_HART + 1];

void cap_lock_init(void)
{
	mcslock_init(&lock);
}

bool cap_lock_acquire(void)
{
#ifndef NOPREEMPT
	return mcslock_try_acquire(&lock, &nodes[csrr(mhartid)]);
#else
	mcslock_acquire(&lock, &nodes[csrr(mhartid)]);
	return true;
#endif
}

void cap_lock_release(void)
{
	mcslock_release(&lock, &nodes[csrr(mhartid)]);
}
#endif
