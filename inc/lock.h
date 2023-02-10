#pragma once
#include <stdbool.h>
#include <stdint.h>

struct ticket_lock {
	volatile uint64_t next_tick;
	volatile uint64_t serving;
};

void tl_lock(struct ticket_lock *l);
void tl_unlock(struct ticket_lock *l);
