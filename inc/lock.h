#ifndef __LOCK_H__
#define __LOCK_H__
#include <stdbool.h>
#include <stdint.h>

struct spinlock {
	uint64_t lock;
};

struct ticklock {
	uint64_t next_tick;
	uint64_t serving;
};

void spinlock_lock(struct spinlock *l);
bool spinlock_trylock(struct spinlock *l);
void spinlock_unlock(struct spinlock *l);

void ticklock_lock(struct ticklock *l);
void ticklock_unlock(struct ticklock *l);

#endif /* __LOCK_H__ */
