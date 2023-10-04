#pragma once

#include "cap_types.h"
#include "kassert.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct cte *cte_t;

void ctable_init(void);
cte_t ctable_get(uint64_t pid, uint64_t index);
bool cte_is_empty(cte_t c);
void cte_set_next(cte_t c, cte_t next);
void cte_set_prev(cte_t c, cte_t prev);
void cte_set_cap(cte_t c, cap_t cap);
cte_t cte_next(cte_t c);
cte_t cte_prev(cte_t c);
cap_t cte_cap(cte_t c);
uint64_t cte_pid(cte_t c);
void cte_move(cte_t src, cte_t dst, cap_t *cap);
cap_t cte_delete(cte_t c);
void cte_insert(cte_t c, cap_t cap, cte_t prev);
