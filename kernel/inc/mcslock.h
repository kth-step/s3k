#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct qnode {
	struct qnode *prev, *next;
} qnode_t;

typedef struct mcslock {
	struct qnode tail;
	struct qnode *nodes;
} mcslock_t;

void mcslock_init(mcslock_t *lock, qnode_t *nodes);
bool mcslock_try_acquire(mcslock_t *lock, uint64_t me);
void mcslock_acquire(mcslock_t *lock, uint64_t me);
void mcslock_release(mcslock_t *lock, uint64_t me);
