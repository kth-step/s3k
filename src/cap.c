#include "cap.h"

union cap cap_time(uint64_t hartid, uint64_t begin, uint64_t end)
{
	return (union cap){.time = {CAPTY_TIME, 0, hartid, begin, begin, end}};
}

union cap cap_memory(uint64_t begin, uint64_t end, uint64_t offset, uint64_t rwx)
{
	return (union cap){.memory = {CAPTY_MEMORY, false, rwx, offset, begin, begin, end}};
}

union cap cap_pmp(uint64_t addr, uint64_t rwx)
{
	return (union cap){.pmp = {CAPTY_PMP, 0x18 | rwx, addr, 0}};
}

union cap cap_monitor(uint64_t begin, uint64_t end)
{
	return (union cap){.monitor = {CAPTY_MONITOR, 0, begin, begin, end}};
}

union cap cap_channel(uint64_t begin, uint64_t end)
{
	return (union cap){.channel = {CAPTY_CHANNEL, 0, begin, begin, end}};
}

union cap cap_socket(uint64_t channel, uint64_t tag)
{
	return (union cap){.socket = {CAPTY_SOCKET, 0, channel, tag}};
}

static bool cap_time_derive_time(struct time parent, struct time child)
{
	return parent.free <= child.begin && child.end <= parent.end && child.begin == child.free &&
	       child.begin < child.end && child._padd == 0;
}

static bool cap_memory_derive_memory(struct memory parent, struct memory child)
{
	return parent.free <= child.begin && child.end <= parent.end && child.begin == child.free &&
	       parent.offset == child.offset && child.begin <= child.end &&
	       (parent.rwx & child.rwx) == child.rwx && parent.lock == 0;
}

static bool cap_memory_derive_pmp(struct memory parent, struct pmp child)
{
	uint64_t pmp_begin = pmp_napot_begin(child.addr);
	uint64_t pmp_end = pmp_napot_end(child.addr);
	uint64_t rwx = child.cfg & 0x7;
	uint64_t mode = child.cfg >> 3;
	uint64_t mem_free = ((uint64_t)parent.offset << 28) | (parent.free << 12);
	uint64_t mem_end = ((uint64_t)parent.offset << 28) + ((parent.end + 1) << 12);
	return mem_free <= pmp_begin && pmp_end <= mem_end && ((parent.rwx & rwx) == rwx) &&
	       mode == 0x3;
}

static bool cap_monitor_derive_monitor(struct monitor parent, struct monitor child)
{
	return parent.free <= child.begin && child.end <= parent.end && child.begin == child.free &&
	       child.begin < child.end && child._padd == 0;
}

static bool cap_channel_derive_channel(struct channel parent, struct channel child)
{
	return parent.free <= child.begin && child.end <= parent.end && child.begin == child.free &&
	       child.begin < child.end && child._padd == 0;
}

static bool cap_channel_derive_socket(struct channel parent, struct socket child)
{
	return parent.free <= child.channel && child.channel < parent.end && child.tag == 0 &&
	       child._padd == 0;
}

static bool cap_socket_derive_socket(struct socket parent, struct socket child)
{
	return parent.channel == child.channel && parent.tag == 0 && child.tag > 0 &&
	       child._padd == 0;
}

bool cap_time_derive(union cap parent, union cap child)
{
	return parent.type == CAPTY_TIME && child.type == CAPTY_TIME &&
	       cap_time_derive_time(parent.time, child.time);
}

bool cap_memory_derive(union cap parent, union cap child)
{
	return (parent.type == CAPTY_MEMORY && child.type == CAPTY_MEMORY &&
		cap_memory_derive_memory(parent.memory, child.memory)) ||
	       (parent.type == CAPTY_MEMORY && child.type == CAPTY_PMP &&
		cap_memory_derive_pmp(parent.memory, child.pmp));
}

bool cap_monitor_derive(union cap parent, union cap child)
{
	return parent.type == CAPTY_MONITOR && child.type == CAPTY_MONITOR &&
	       cap_monitor_derive_monitor(parent.monitor, child.monitor);
}

bool cap_channel_derive(union cap parent, union cap child)
{
	return (parent.type == CAPTY_CHANNEL && child.type == CAPTY_CHANNEL &&
		cap_channel_derive_channel(parent.channel, child.channel)) ||
	       (parent.type == CAPTY_CHANNEL && child.type == CAPTY_SOCKET &&
		cap_channel_derive_socket(parent.channel, child.socket));
}

bool cap_socket_derive(union cap parent, union cap child)
{
	return (parent.type == CAPTY_SOCKET && child.type == CAPTY_SOCKET &&
		cap_socket_derive_socket(parent.socket, child.socket));
}

static bool cap_time_parent_time(struct time parent, struct time child)
{
	return parent.begin <= child.begin && child.end <= parent.free;
}

static bool cap_memory_parent_memory(struct memory parent, struct memory child)
{
	return parent.offset == child.offset && parent.begin <= child.begin &&
	       child.end <= parent.free;
}

static bool cap_memory_parent_pmp(struct memory parent, struct pmp child)
{
	uint64_t pmp_begin = pmp_napot_begin(child.addr);
	uint64_t pmp_end = pmp_napot_end(child.addr);
	uint64_t rwx = child.cfg & 0x7;
	uint64_t mode = child.cfg >> 3;
	uint64_t mem_free = ((uint64_t)parent.offset << 28) | (parent.free << 12);
	uint64_t mem_end = ((uint64_t)parent.offset << 28) + ((parent.end + 1) << 12);
	return mem_free <= pmp_begin && pmp_end <= mem_end && ((parent.rwx & rwx) == rwx) &&
	       mode == 0x3;
}

static bool cap_monitor_parent_monitor(struct monitor parent, struct monitor child)
{
	return parent.begin <= child.begin && child.end <= parent.free;
}

static bool cap_channel_parent_channel(struct channel parent, struct channel child)
{
	return parent.begin <= child.begin && child.end <= parent.free;
}

static bool cap_channel_parent_socket(struct channel parent, struct socket child)
{
	return parent.begin <= child.channel && child.channel <= parent.free;
}

static bool cap_socket_parent_socket(struct socket parent, struct socket child)
{
	return parent.tag == 0 && parent.channel == child.channel;
}

bool cap_time_parent(union cap parent, union cap child)
{
	return parent.type == CAPTY_TIME && child.type == CAPTY_TIME &&
	       cap_time_parent_time(parent.time, child.time);
}

bool cap_memory_parent(union cap parent, union cap child)
{
	return (parent.type == CAPTY_MEMORY && child.type == CAPTY_MEMORY &&
		cap_memory_parent_memory(parent.memory, child.memory)) ||
	       (parent.type == CAPTY_MEMORY && child.type == CAPTY_PMP &&
		cap_memory_parent_pmp(parent.memory, child.pmp));
}

bool cap_monitor_parent(union cap parent, union cap child)
{
	return parent.type == CAPTY_MONITOR && child.type == CAPTY_MONITOR &&
	       cap_monitor_parent_monitor(parent.monitor, child.monitor);
}

bool cap_channel_parent(union cap parent, union cap child)
{
	return (parent.type == CAPTY_CHANNEL && child.type == CAPTY_CHANNEL &&
		cap_channel_parent_channel(parent.channel, child.channel)) ||
	       (parent.type == CAPTY_CHANNEL && child.type == CAPTY_SOCKET &&
		cap_channel_parent_socket(parent.channel, child.socket));
}

bool cap_socket_parent(union cap parent, union cap child)
{
	return parent.type == CAPTY_SOCKET && child.type == CAPTY_SOCKET &&
	       cap_socket_parent_socket(parent.socket, child.socket);
}

uint64_t pmp_napot_addr(uint64_t begin, uint64_t end)
{
	return (begin | (((end - begin) - 1) >> 1)) >> 2;
}

uint64_t pmp_napot_begin(uint64_t addr)
{
	return ((addr + 1) & addr) << 2;
}

uint64_t pmp_napot_end(uint64_t addr)
{
	return (((addr + 1) | addr) + 1) << 2;
}
