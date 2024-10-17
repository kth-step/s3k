/**
 * A counting semaphore based on a ticket lock.
 */
#pragma once
#include <stdint.h>

typedef struct semaphore {
	uint64_t released;
	uint64_t acquired;
} semaphore_t;

/**
 * Initialize the semaphore with n tickets.
 */
void semaphore_init(semaphore_t *sem, uint64_t tickets);

/**
 * Acquire n tickets and wait for release.
 */
void semaphore_acquire_n(semaphore_t *sem, uint64_t n);

/**
 * Release n tickets.
 */
void semaphore_release_n(semaphore_t *sem, uint64_t n);

/**
 * Acquire 1 ticket and wait for release.
 */
void semaphore_acquire(semaphore_t *sem);

/**
 * Release 1 ticket.
 */
void semaphore_release(semaphore_t *sem);
