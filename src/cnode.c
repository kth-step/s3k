/* See LICENSE file for copyright and license details. */
#include "cnode.h"

#include "cap.h"
#include "common.h"
#include "consts.h"
#include "kassert.h"

struct cnode {
	uint32_t prev, next;
	union cap cap;
};

static volatile struct cnode cnodes[NPROC * NCAP + 1];

static void _insert(uint32_t curr, union cap cap, uint32_t prev)
{
	uint32_t next = cnodes[prev].next;
	cnodes[curr].prev = prev;
	cnodes[curr].next = next;
	cnodes[prev].next = curr;
	cnodes[next].prev = curr;
	cnodes[curr].cap = cap;
}

void _delete(uint32_t curr)
{
	uint32_t prev = cnodes[curr].prev;
	uint32_t next = cnodes[curr].next;
	cnodes[next].prev = prev;
	cnodes[prev].next = next;

	cnodes[curr].cap.raw = 0;
	cnodes[curr].prev = 0;
	cnodes[curr].next = 0;
}

static void _move(uint32_t src, uint32_t dst)
{
	uint32_t prev = cnodes[src].prev;
	union cap cap = cnodes[src].cap;
	uint32_t next = cnodes[src].next;

	// update destination node
	cnodes[dst].prev = prev;
	cnodes[dst].cap = cap;
	cnodes[dst].next = next;

	// update previous and next node
	cnodes[prev].next = dst;
	cnodes[next].prev = dst;

	cnodes[src].cap.raw = 0;
	cnodes[src].prev = 0;
	cnodes[src].next = 0;
}

void cnode_init(const union cap *caps, size_t size)
{
	// zero cnodes
	for (int i = 0; i < NPROC * NCAP; ++i)
		cnodes[i] = (struct cnode){ 0 };
	// Initialize root node
	cnode_handle_t root = NPROC * NCAP;
	cnodes[root].prev = root;
	cnodes[root].next = root;
	// Add initial nodes
	int prev = root;
	for (cnode_handle_t i = 0; i < size; i++) {
		_insert(i, caps[i], prev);
		prev = i;
	}
}

// Handle is just index to corresponding element in cnodes
cnode_handle_t cnode_get_handle(cnode_handle_t pid, cnode_handle_t idx)
{
	kassert(pid < NPROC);
	return pid * NCAP + (idx % NCAP);
}

cnode_handle_t cnode_get_pid(cnode_handle_t handle)
{
	kassert(handle < NPROC * NCAP);
	return handle / NCAP;
}

cnode_handle_t cnode_get_next(cnode_handle_t handle)
{
	kassert(handle < NPROC * NCAP);
	return cnodes[handle].next;
}

union cap cnode_get_cap(cnode_handle_t handle)
{
	kassert(handle < NPROC * NCAP);
	return cnodes[handle].cap;
}

void cnode_set_cap(cnode_handle_t handle, union cap cap)
{
	kassert(cap.raw != 0);
	kassert(cnode_contains(handle));
	cnodes[handle].cap = cap;
}

bool cnode_contains(cnode_handle_t handle)
{
	kassert(handle < NPROC * NCAP);
	return cnodes[handle].cap.raw != 0;
}

void cnode_insert(cnode_handle_t curr, union cap cap, cnode_handle_t prev)
{
	kassert(curr < NPROC * NCAP);
	kassert(prev < NPROC * NCAP);
	kassert(cap.raw != 0);
	kassert(!cnode_contains(curr));
	kassert(cnode_contains(prev));

	_insert(curr, cap, prev);
}

void cnode_move(cnode_handle_t src, cnode_handle_t dst)
{
	kassert(src < NPROC * NCAP);
	kassert(dst < NPROC * NCAP);
	kassert(cnode_contains(src));
	kassert(!cnode_contains(dst));
	_move(src, dst);
}

void cnode_delete(cnode_handle_t curr)
{
	kassert(curr < NPROC * NCAP);
	kassert(cnode_contains(curr));

	_delete(curr);
}

bool cnode_delete_if(cnode_handle_t curr, cnode_handle_t prev)
{
	kassert(curr < NPROC * NCAP);
	if (!cnode_contains(curr) || cnodes[curr].prev != prev)
		return false;
	_delete(curr);
	return true;
}
