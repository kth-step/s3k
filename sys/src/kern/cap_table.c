#include "kern/cap_table.h"

#include "kern/cap_util.h"
#include "kern/kassert.h"

extern cap_t init_caps[];

extern struct cte ctable[];

static uint32_t offset(cte_t c)
{
	return (uint32_t)(c - ctable);
}

cte_t ctable_get(uint64_t pid, uint64_t index)
{
	KASSERT(pid < S3K_PROC_CNT);
	KASSERT(index < S3K_CAP_CNT);
	return &ctable[pid * S3K_CAP_CNT + index];
}

bool cte_is_empty(cte_t c)
{
	return c->cap.type == 0;
}

void cte_set_next(cte_t c, cte_t next)
{
	c->next = offset(next);
}

void cte_set_prev(cte_t c, cte_t prev)
{
	c->prev = offset(prev);
}

void cte_set_cap(cte_t c, cap_t cap)
{
	c->cap = cap;
}

cte_t cte_next(cte_t c)
{
	return &ctable[c->next];
}

cte_t cte_prev(cte_t c)
{
	return &ctable[c->prev];
}

cap_t cte_cap(cte_t c)
{
	return c->cap;
}

uint64_t cte_pid(cte_t c)
{
	return offset(c) / S3K_CAP_CNT;
}

void cte_move(cte_t src, cte_t dst)
{
	if (src == dst)
		return;
	cte_set_cap(dst, cte_cap(src));
	cte_set_cap(src, (cap_t){0});
	cte_set_prev(dst, cte_prev(src));
	cte_set_next(dst, cte_next(src));
	cte_prev(dst)->next = offset(dst);
	cte_next(dst)->prev = offset(dst);
}

cap_t cte_delete(cte_t c)
{
	cap_t cap = cte_cap(c);
	cte_set_cap(c, (cap_t){0});
	cte_set_next(cte_prev(c), cte_next(c));
	cte_set_prev(cte_next(c), cte_prev(c));
	return cap;
}

void cte_insert(cte_t c, cap_t cap, cte_t prev)
{
	cte_set_prev(c, prev);
	cte_set_next(c, cte_next(prev));
	cte_set_next(cte_prev(c), c);
	cte_set_prev(cte_next(c), c);
	cte_set_cap(c, cap);
}
