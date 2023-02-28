/**
 * @file lock.h
 * @brief Ticket lock
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */
#ifndef __LOCK_H__
#define __LOCK_H__

#include <stdbool.h>
#include <stdint.h>

struct ticket_lock {
	volatile uint64_t next_tick;
	volatile uint64_t serving;
};

void tl_lock(struct ticket_lock *l);
void tl_unlock(struct ticket_lock *l);

#endif /* __LOCK_H__ */
