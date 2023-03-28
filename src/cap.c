#include "cap.h"

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

bool cap_time_parent(union cap parent, union cap child)
{
	return parent.type == CAPTY_TIME && child.type == CAPTY_TIME
	       && parent.time.begin <= child.time.begin
	       && child.time.end <= parent.time.end
	       && child.time.hartid == parent.time.hartid;
}

bool cap_memory_parent(union cap parent, union cap child)
{
	if (parent.type == CAPTY_MEMORY && child.type == CAPTY_MEMORY) {
		return parent.memory.offset == child.memory.offset
		       && parent.memory.begin <= child.memory.begin
		       && child.memory.end <= parent.memory.end;
	}
	if (parent.type == CAPTY_MEMORY && child.type == CAPTY_PMP) {
		uint64_t pmp_begin = pmp_napot_begin(child.pmp.addr);
		uint64_t pmp_end = pmp_napot_end(child.pmp.addr);
		uint64_t mem_begin = ((uint64_t)parent.memory.offset << 27)
				     + (parent.memory.begin << 12);
		uint64_t mem_end = ((uint64_t)parent.memory.offset << 27)
				   + (parent.memory.end << 12);
		return mem_begin <= pmp_begin && pmp_end <= mem_end;
	}
	return false;
}

bool cap_monitor_parent(union cap parent, union cap child)
{
	return parent.type == CAPTY_MONITOR && child.type == CAPTY_MONITOR
	       && parent.monitor.begin <= child.monitor.begin
	       && child.monitor.end <= parent.monitor.end;
}

bool cap_channel_parent(union cap parent, union cap child)
{
	if (parent.type == CAPTY_CHANNEL && child.type == CAPTY_CHANNEL)
		return parent.channel.begin <= child.channel.begin
		       && child.channel.end <= parent.channel.end;
	if (parent.type == CAPTY_CHANNEL && child.type == CAPTY_SOCKET)
		return parent.channel.begin <= child.socket.channel
		       && child.socket.channel < parent.channel.end;
	return false;
}

bool cap_socket_parent(union cap parent, union cap child)
{
	return parent.type == CAPTY_SOCKET && child.type == CAPTY_SOCKET
	       && parent.socket.tag == 0
	       && parent.socket.channel == child.socket.channel;
}

bool cap_time_derive(union cap parent, union cap child)
{
	return parent.type == CAPTY_TIME && child.type == CAPTY_TIME
	       && parent.time.free == child.time.begin
	       && parent.time.free == child.time.free
	       && child.time.end <= parent.time.end
	       && child.time.hartid == parent.time.hartid;
}

bool cap_memory_derive(union cap parent, union cap child)
{
	if (parent.type == CAPTY_MEMORY && child.type == CAPTY_MEMORY) {
		return parent.memory.offset == child.memory.offset
		       && parent.memory.free == child.memory.begin
		       && parent.memory.free == child.memory.free
		       && child.memory.end <= parent.memory.end;
	}
	if (parent.type == CAPTY_MEMORY && child.type == CAPTY_PMP) {
		uint64_t pmp_begin = pmp_napot_begin(child.pmp.addr);
		uint64_t pmp_end = pmp_napot_end(child.pmp.addr);
		uint64_t pmp_rwx = child.pmp.cfg & 0x7;
		uint64_t pmp_mode = child.pmp.cfg >> 3;
		uint64_t mem_free = ((uint64_t)parent.memory.offset << 27)
				    + (parent.memory.free << 12);
		uint64_t mem_end = ((uint64_t)parent.memory.offset << 27)
				   + (parent.memory.end << 12);
		uint64_t mem_rwx = parent.memory.rwx;
		return mem_free <= pmp_begin && pmp_end <= mem_end
		       && pmp_mode == 0x3 && (mem_rwx & pmp_rwx) == pmp_rwx;
	}
	return false;
}

bool cap_monitor_derive(union cap parent, union cap child)
{
	return parent.type == CAPTY_MONITOR && child.type == CAPTY_MONITOR
	       && parent.monitor.free == child.monitor.begin
	       && parent.monitor.free == child.monitor.free
	       && child.monitor.end <= parent.monitor.end;
}

bool cap_channel_derive(union cap parent, union cap child)
{
	if (parent.type == CAPTY_CHANNEL && child.type == CAPTY_CHANNEL)
		return parent.channel.free == child.channel.begin
		       && parent.channel.free == child.channel.free
		       && child.channel.end <= parent.channel.end;
	if (parent.type == CAPTY_CHANNEL && child.type == CAPTY_SOCKET)
		return parent.channel.free == child.socket.channel
		       && child.socket.channel < parent.channel.end;
	return false;
}

bool cap_socket_derive(union cap parent, union cap child)
{
	return parent.type == CAPTY_SOCKET && child.type == CAPTY_SOCKET
	       && parent.socket.tag == 0 && child.socket.tag > 0
	       && parent.socket.channel == child.socket.channel;
}
