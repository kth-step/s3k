#pragma once
#include <stdbool.h>

typedef struct taslock {
	int lock;
} taslock_t;

void taslock_init(taslock_t *l);
bool taslock_acquire(taslock_t *l);
bool taslock_try_acquire(taslock_t *l);
void taslock_release(taslock_t *l);
