#include "cap/table.h"

#include "cap/init.h"
#include "cap/util.h"
#include "kassert.h"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

struct cte {
	uint32_t prev, next;
	cap_t cap;
};

static struct cte ctable[NPROC * NCAP];

static uint32_t offset(cte_t c)
{
	return (uint32_t)(c - ctable);
}

void ctable_init(void)
{
	const cap_t init_caps[] = INIT_CAPS;
	cte_t prev = ctable;
	kprintf(0, "# Initial capabilities:\n");
	for (unsigned int i = 0; i < ARRAY_SIZE(init_caps); ++i) {
		if (init_caps[i].type == CAPTY_NONE)
			continue;
		cte_insert(&ctable[i], init_caps[i], prev);

		char buf[128];
		cap_snprint(buf, 128, init_caps[i]);
		kprintf(0, "#\t%d: %s\n", i, buf);
	}
}

cte_t ctable_get(uint64_t pid, uint64_t index)
{
	KASSERT(pid < NPROC);
	KASSERT(index < NCAP);
	return &ctable[pid * NCAP + index];
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
	return offset(c) / NCAP;
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
