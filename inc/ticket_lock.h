/**
 * @file lock.h
 *
 * This header file contains the declarations for the ticket lock
 * synchronization primitive, which allows multiple threads to acquire
 * a lock in a fair and orderly manner. The ticket lock works by assigning
 * each thread a unique ticket number, and then serving threads in the order
 * of their ticket numbers. This ensures that threads acquire the lock in
 * the order that they request it, preventing starvation and ensuring
 * fairness.
 *
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */
#ifndef __LOCK_H__
#define __LOCK_H__

#include <stdint.h>

struct ticket_lock {
	/// next ticket number to be issued.
	int next_ticket;
	/// ticket number currently being served.
	volatile int serving_ticket;
};

/**
 * Acquire a ticket lock.
 *
 * @param lock Pointer to the ticket lock to acquire.
 */
void tl_acq(struct ticket_lock *lock);

/**
 * Release a ticket lock.
 *
 * @param lock Pointer to the ticket lock to release.
 */
void tl_rel(struct ticket_lock *lock);

#endif /* __LOCK_H__ */
