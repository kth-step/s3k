/* See LICENSE file for copyright and license details. */
#include "cnode.h"

#include "cap.h"
#include "common.h"
#include "consts.h"
#include "kassert.h"

struct cnode {
	cnode_handle_t prev, next;
	union cap cap;
};

static volatile struct cnode _cnodes[NPROC * NCAP + 1];

static void _insert(cnode_handle_t curr, union cap cap, cnode_handle_t prev)
{
	cnode_handle_t next = _cnodes[prev].next;
	_cnodes[curr].prev = prev;
	_cnodes[curr].next = next;
	_cnodes[prev].next = curr;
	_cnodes[next].prev = curr;
	_cnodes[curr].cap = cap;
}

void _delete(cnode_handle_t curr)
{
	cnode_handle_t prev = _cnodes[curr].prev;
	cnode_handle_t next = _cnodes[curr].next;
	_cnodes[next].prev = prev;
	_cnodes[prev].next = next;

	_cnodes[curr].cap.raw = 0;
	_cnodes[curr].prev = 0;
	_cnodes[curr].next = 0;
}

static void _move(cnode_handle_t src, cnode_handle_t dst)
{
	cnode_handle_t prev = _cnodes[src].prev;
	union cap cap = _cnodes[src].cap;
	cnode_handle_t next = _cnodes[src].next;

	// update destination node
	_cnodes[dst].prev = prev;
	_cnodes[dst].cap = cap;
	_cnodes[dst].next = next;

	// update previous and next node
	_cnodes[prev].next = dst;
	_cnodes[next].prev = dst;

	_cnodes[src].cap.raw = 0;
	_cnodes[src].prev = 0;
	_cnodes[src].next = 0;
}

static bool _contains(cnode_handle_t curr)
{
	return _cnodes[curr].cap.raw != 0;
}

void cnode_init(const union cap *caps, size_t size)
{
	// zero cnodes
	for (int i = 0; i < NPROC * NCAP; ++i)
		_cnodes[i] = (struct cnode){ 0 };
	// Initialize root node
	cnode_handle_t root = NPROC * NCAP;
	_cnodes[root].prev = root;
	_cnodes[root].next = root;
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
	return _cnodes[handle].next;
}

union cap cnode_get_cap(cnode_handle_t handle)
{
	kassert(handle < NPROC * NCAP);
	return _cnodes[handle].cap;
}

void cnode_set_cap(cnode_handle_t handle, union cap cap)
{
	kassert(cap.raw != 0);
	kassert(cnode_contains(handle));
	_cnodes[handle].cap = cap;
}

bool cnode_contains(cnode_handle_t handle)
{
	kassert(handle < NPROC * NCAP);
	return _contains(handle);
}

void cnode_insert(cnode_handle_t curr, union cap cap, cnode_handle_t prev)
{
	kassert(curr < NPROC * NCAP);
	kassert(prev < NPROC * NCAP);
	kassert(cap.raw != 0);
	kassert(!_contains(curr));
	kassert(_contains(prev));

	_insert(curr, cap, prev);
}

void cnode_move(cnode_handle_t src, cnode_handle_t dst)
{
	kassert(src < NPROC * NCAP);
	kassert(dst < NPROC * NCAP);
	kassert(_contains(src));
	kassert(!_contains(dst));
	_move(src, dst);
}

void cnode_delete(cnode_handle_t curr)
{
	kassert(curr < NPROC * NCAP);
	kassert(_contains(curr));

	_delete(curr);
}

bool cnode_delete_if(cnode_handle_t curr, cnode_handle_t prev)
{
	kassert(curr < NPROC * NCAP);
	if (!_contains(curr) || _cnodes[curr].prev != prev)
		return false;
	_delete(curr);
	return true;
}
