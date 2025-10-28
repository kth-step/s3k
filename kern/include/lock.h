#pragma once

#include "types.h"

/**
 * Initializes the lock.
 */
void lock_init(void);

/**
 * Acquires the lock.
 */
bool lock_acquire(bool preemptable);

/**
 * Releases the lock.
 */
void lock_release(void);
