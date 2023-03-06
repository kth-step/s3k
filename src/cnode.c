/* See LICENSE file for copyright and license details. */
#include "cnode.h"

#include "cap.h"
#include "common.h"
#include "consts.h"

struct cnode {
	uint32_t prev, next;
	uint64_t raw_cap;
};

static volatile struct cnode cnodes[NPROC * NCAP + 1];

void cnode_init(void)
{
	cnode_handle_t root = cnode_get_root_handle();
	cnodes[root].prev = root;
	cnodes[root].next = root;
	cnodes[root].raw_cap = -1;
}

// Handle is just index to corresponding element in cnodes
cnode_handle_t cnode_get_handle(cnode_handle_t pid, cnode_handle_t idx)
{
	assert(pid < NPROC);
	return pid * NCAP + (idx % NCAP);
}

cnode_handle_t cnode_get_root_handle(void)
{
	return NPROC * NCAP;
}

cnode_handle_t cnode_get_pid(cnode_handle_t handle)
{
	assert(handle < NPROC * NCAP);
	return handle / NCAP;
}

cnode_handle_t cnode_get_next(cnode_handle_t handle)
{
	assert(handle < NPROC * NCAP);
	return cnodes[handle].next;
}

union cap cnode_get_cap(cnode_handle_t handle)
{
	assert(handle < NPROC * NCAP);
	return (union cap){.raw = cnodes[handle].raw_cap};
}

void cnode_set_cap(cnode_handle_t handle, union cap cap)
{
	assert(cap.raw != 0);
	assert(cnode_contains(handle));
	cnodes[handle].raw_cap = cap.raw;
}

bool cnode_contains(cnode_handle_t handle)
{
	assert(handle <= NPROC * NCAP);
	return cnodes[handle].raw_cap != 0;
}

void cnode_insert(cnode_handle_t handle, union cap cap, cnode_handle_t prev_handle)
{
	assert(handle < NPROC * NCAP);
	assert(prev_handle <= NPROC * NCAP);
	assert(cap.raw != 0);
	assert(!cnode_contains(handle));
	assert(cnode_contains(prev_handle));

	cnode_handle_t next_handle = cnodes[prev_handle].next;
	cnodes[handle].prev = prev_handle;
	cnodes[handle].next = next_handle;
	cnodes[prev_handle].next = handle;
	cnodes[next_handle].prev = handle;
	cnodes[handle].raw_cap = cap.raw;
}

void cnode_move(cnode_handle_t src_handle, cnode_handle_t dst_handle)
{
	assert(src_handle < NPROC * NCAP);
	assert(dst_handle < NPROC * NCAP);
	assert(cnode_contains(src_handle));
	assert(!cnode_contains(dst_handle));

	cnodes[dst_handle].raw_cap = cnodes[src_handle].raw_cap;
	cnodes[dst_handle].prev = cnodes[src_handle].prev;
	cnodes[dst_handle].next = cnodes[src_handle].next;
	cnodes[src_handle].raw_cap = 0;
	cnodes[src_handle].prev = 0;
	cnodes[src_handle].next = 0;
}

void cnode_delete(cnode_handle_t handle)
{
	assert(handle < NPROC * NCAP);
	assert(cnode_contains(handle));

	cnode_handle_t prev_handle = cnodes[handle].prev;
	cnode_handle_t next_handle = cnodes[handle].next;
	cnodes[prev_handle].next = next_handle;
	cnodes[next_handle].prev = prev_handle;
	cnodes[handle].raw_cap = 0;
	cnodes[handle].prev = 0;
	cnodes[handle].next = 0;
}

bool cnode_delete_if(cnode_handle_t handle, cnode_handle_t prev_handle)
{
	assert(handle < NPROC * NCAP);
	if (!cnode_contains(handle) || cnodes[handle].prev != prev_handle)
		return false;
	cnode_handle_t next_handle = cnodes[handle].next;
	cnodes[prev_handle].next = next_handle;
	cnodes[next_handle].prev = prev_handle;
	cnodes[handle].raw_cap = 0;
	cnodes[handle].prev = 0;
	cnodes[handle].next = 0;
	return true;
}
