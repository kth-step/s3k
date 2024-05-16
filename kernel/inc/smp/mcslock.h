#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct qnode {
	struct qnode *next;
	struct qnode *prev;
	uint64_t state;
} qnode_t;

typedef struct mcslock {
	struct qnode tail;
} mcslock_t;

void mcslock_init(mcslock_t *lock);
bool mcslock_try_acquire(mcslock_t *lock, qnode_t *qnode);
void mcslock_acquire(mcslock_t *lock, qnode_t *qnode);
void mcslock_release(mcslock_t *lock, qnode_t *qnode);
