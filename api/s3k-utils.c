#include "s3k.h"

uint64_t s3k_pmp_napot_addr(uint64_t begin, uint64_t end)
{
	return (begin | (((end - begin) - 1) >> 1)) >> 2;
}

uint64_t s3k_pmp_napot_begin(uint64_t addr)
{
	return ((addr + 1) & addr) << 2;
}

uint64_t s3k_pmp_napot_end(uint64_t addr)
{
	return (((addr + 1) | addr) + 1) << 2;
}

union s3k_cap s3k_time(uint64_t hartid, uint64_t begin, uint64_t end)
{
	return (union s3k_cap){
		.time = {S3K_CAPTY_TIME, 0, hartid, begin, begin, end}
	};
}

union s3k_cap s3k_memory(uint64_t begin, uint64_t end, uint64_t offset,
			 uint64_t rwx)
{
	return (union s3k_cap){
		.memory
		= {S3K_CAPTY_MEMORY, false, rwx, offset, begin, begin, end}
	};
}

union s3k_cap s3k_pmp(uint64_t addr, uint64_t rwx)
{
	return (union s3k_cap){
		.pmp = {S3K_CAPTY_PMP, addr, 0x18 | rwx}
	};
}

union s3k_cap s3k_monitor(uint64_t begin, uint64_t end)
{
	return (union s3k_cap){
		.monitor = {S3K_CAPTY_MONITOR, 0, begin, begin, end}
	};
}

union s3k_cap s3k_channel(uint64_t begin, uint64_t end)
{
	return (union s3k_cap){
		.channel = {S3K_CAPTY_CHANNEL, 0, begin, begin, end}
	};
}

union s3k_cap s3k_socket(uint64_t channel, uint64_t tag)
{
	return (union s3k_cap){
		.socket = {S3K_CAPTY_SOCKET, 0, channel, tag}
	};
}

bool s3k_can_derive(union s3k_cap parent, union s3k_cap child)
{
	return s3k_time_derive(parent, child)
	       || s3k_memory_derive(parent, child)
	       || s3k_monitor_derive(parent, child)
	       || s3k_channel_derive(parent, child)
	       || s3k_socket_derive(parent, child);
}

bool s3k_is_parent(union s3k_cap parent, union s3k_cap child)
{
	return s3k_time_parent(parent, child)
	       || s3k_memory_parent(parent, child)
	       || s3k_monitor_parent(parent, child)
	       || s3k_channel_parent(parent, child)
	       || s3k_socket_parent(parent, child);
}

bool s3k_time_parent(union s3k_cap parent, union s3k_cap child)
{
	return parent.type == S3K_CAPTY_TIME && child.type == S3K_CAPTY_TIME
	       && parent.time.begin <= child.time.begin
	       && child.time.end <= parent.time.end
	       && child.time.hartid == parent.time.hartid;
}

bool s3k_memory_parent(union s3k_cap parent, union s3k_cap child)
{
	if (parent.type == S3K_CAPTY_MEMORY && child.type == S3K_CAPTY_MEMORY) {
		return parent.memory.offset == child.memory.offset
		       && parent.memory.begin <= child.memory.begin
		       && child.memory.end <= parent.memory.end;
	}
	if (parent.type == S3K_CAPTY_MEMORY && child.type == S3K_CAPTY_PMP) {
		uint64_t pmp_begin = s3k_pmp_napot_begin(child.pmp.addr);
		uint64_t pmp_end = s3k_pmp_napot_end(child.pmp.addr);
		uint64_t mem_begin = ((uint64_t)parent.memory.offset << 27)
				     + (parent.memory.begin << 12);
		uint64_t mem_end = ((uint64_t)parent.memory.offset << 27)
				   + (parent.memory.end << 12);
		return mem_begin <= pmp_begin && pmp_end <= mem_end;
	}
	return false;
}

bool s3k_monitor_parent(union s3k_cap parent, union s3k_cap child)
{
	return parent.type == S3K_CAPTY_MONITOR
	       && child.type == S3K_CAPTY_MONITOR
	       && parent.monitor.begin <= child.monitor.begin
	       && child.monitor.end <= parent.monitor.end;
}

bool s3k_channel_parent(union s3k_cap parent, union s3k_cap child)
{
	if (parent.type == S3K_CAPTY_CHANNEL && child.type == S3K_CAPTY_CHANNEL)
		return parent.channel.begin <= child.channel.begin
		       && child.channel.end <= parent.channel.end;
	if (parent.type == S3K_CAPTY_CHANNEL && child.type == S3K_CAPTY_SOCKET)
		return parent.channel.begin <= child.socket.channel
		       && child.socket.channel < parent.channel.end;
	return false;
}

bool s3k_socket_parent(union s3k_cap parent, union s3k_cap child)
{
	return parent.type == S3K_CAPTY_SOCKET && child.type == S3K_CAPTY_SOCKET
	       && parent.socket.tag == 0
	       && parent.socket.channel == child.socket.channel;
}

bool s3k_time_derive(union s3k_cap parent, union s3k_cap child)
{
	return parent.type == S3K_CAPTY_TIME && child.type == S3K_CAPTY_TIME
	       && parent.time.free == child.time.begin
	       && parent.time.free == child.time.free
	       && child.time.end <= parent.time.end
	       && child.time.hartid == parent.time.hartid;
}

bool s3k_memory_derive(union s3k_cap parent, union s3k_cap child)
{
	if (parent.type == S3K_CAPTY_MEMORY && child.type == S3K_CAPTY_MEMORY) {
		return parent.memory.offset == child.memory.offset
		       && parent.memory.free == child.memory.begin
		       && parent.memory.free == child.memory.free
		       && child.memory.end <= parent.memory.end;
	}
	if (parent.type == S3K_CAPTY_MEMORY && child.type == S3K_CAPTY_PMP) {
		uint64_t pmp_begin = s3k_pmp_napot_begin(child.pmp.addr);
		uint64_t pmp_end = s3k_pmp_napot_end(child.pmp.addr);
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

bool s3k_monitor_derive(union s3k_cap parent, union s3k_cap child)
{
	return parent.type == S3K_CAPTY_MONITOR
	       && child.type == S3K_CAPTY_MONITOR
	       && parent.monitor.free == child.monitor.begin
	       && parent.monitor.free == child.monitor.free
	       && child.monitor.end <= parent.monitor.end;
}

bool s3k_channel_derive(union s3k_cap parent, union s3k_cap child)
{
	if (parent.type == S3K_CAPTY_CHANNEL && child.type == S3K_CAPTY_CHANNEL)
		return parent.channel.free == child.channel.begin
		       && parent.channel.free == child.channel.free
		       && child.channel.end <= parent.channel.end;
	if (parent.type == S3K_CAPTY_CHANNEL && child.type == S3K_CAPTY_SOCKET)
		return parent.channel.free == child.socket.channel
		       && child.socket.channel < parent.channel.end;
	return false;
}

bool s3k_socket_derive(union s3k_cap parent, union s3k_cap child)
{
	return parent.type == S3K_CAPTY_SOCKET && child.type == S3K_CAPTY_SOCKET
	       && parent.socket.tag == 0 && child.socket.tag > 0
	       && parent.socket.channel == child.socket.channel;
}
