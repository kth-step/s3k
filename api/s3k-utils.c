#include "s3k.h"

const char *s3k_error2str(enum s3k_excpt code) {
	switch (code) {
	case S3K_EXCPT_NONE:
		return "S3K_EXCPT_NONE";
	case S3K_EXCPT_EMPTY:
		return "S3K_EXCPT_EMPTY";
	case S3K_EXCPT_COLLISION:
		return "S3K_EXCPT_COLLISION";
	case S3K_EXCPT_DERIVATION:
		return "S3K_EXCPT_DERIVATION";
	case S3K_EXCPT_PREEMPTED:
		return "S3K_EXCPT_PREEMPETED";
	case S3K_EXCPT_SUSPENDED:
		return "S3K_EXCPT_SUSPENDED";
	case S3K_EXCPT_MPID:
		return "S3K_EXCPT_MPID";
	case S3K_EXCPT_MBUSY:
		return "S3K_EXCPT_MBUSY";
	case S3K_EXCPT_UNIMPLEMENTED:
		return "S3K_EXCPT_UNIMPLEMENTED";
	case S3K_EXCPT_INVALID_CAP:
		return "S3K_EXCPT_INVALID_CAP";
	case S3K_EXCPT_NO_RECEIVER:
		return "S3K_EXCPT_NO_RECEIVER";
	case S3K_EXCPT_SEND_CAP:
		return "S3K_EXCPT_SEND_CAP";
	default:
		return "UNKNOWN";
	}
}

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

bool s3k_time_derive_time(struct s3k_time parent, struct s3k_time child)
{
	return parent.free == child.begin && child.end <= parent.end
	       && child.begin == child.free && child.begin < child.end
	       && child.hartid == parent.hartid && child._padd == 0;
}

bool s3k_memory_derive_memory(struct s3k_memory parent, struct s3k_memory child)
{
	return parent.free == child.begin && child.end <= parent.end
	       && child.begin == child.free && child.begin < child.end
	       && (parent.rwx & child.rwx) == child.rwx && parent.lock == 0;
}

static bool s3k_memory_derive_pmp(struct s3k_memory parent,
				  struct s3k_pmp child)
{
	uint64_t pmp_begin = s3k_pmp_napot_begin(child.addr);
	uint64_t pmp_end = s3k_pmp_napot_end(child.addr);
	uint64_t rwx = child.cfg & 0x7;
	uint64_t mode = child.cfg >> 3;
	uint64_t mem_free
	    = ((uint64_t)parent.offset << 27) + (parent.free << 12);
	uint64_t mem_end = ((uint64_t)parent.offset << 27) + (parent.end << 12);
	return mem_free <= pmp_begin && pmp_end <= mem_end
	       && ((parent.rwx & rwx) == rwx) && mode == 0x3;
}

bool s3k_monitor_derive_monitor(struct s3k_monitor parent,
				struct s3k_monitor child)
{
	return parent.free == child.begin && child.end <= parent.end
	       && child.begin == child.free && child.begin < child.end;
}

bool s3k_channel_derive_channel(struct s3k_channel parent,
				struct s3k_channel child)
{
	return parent.free == child.begin && child.end <= parent.end
	       && child.begin == child.free && child.begin < child.end
	       && child._padd == 0;
}

bool s3k_channel_derive_socket(struct s3k_channel parent,
			       struct s3k_socket child)
{
	return parent.free == child.channel && child.channel < parent.end
	       && child.tag == 0 && child._padd == 0;
}

bool s3k_socket_derive_socket(struct s3k_socket parent, struct s3k_socket child)
{
	return parent.channel == child.channel && parent.tag == 0
	       && child.tag > 0 && child._padd == 0;
}

bool s3k_time_derive(union s3k_cap parent, union s3k_cap child)
{
	return parent.type == S3K_CAPTY_TIME && child.type == S3K_CAPTY_TIME
	       && s3k_time_derive_time(parent.time, child.time);
}

bool s3k_memory_derive(union s3k_cap parent, union s3k_cap child)
{
	return (parent.type == S3K_CAPTY_MEMORY
		&& child.type == S3K_CAPTY_MEMORY
		&& s3k_memory_derive_memory(parent.memory, child.memory))
	       || (parent.type == S3K_CAPTY_MEMORY
		   && child.type == S3K_CAPTY_PMP
		   && s3k_memory_derive_pmp(parent.memory, child.pmp));
}

bool s3k_monitor_derive(union s3k_cap parent, union s3k_cap child)
{
	return parent.type == S3K_CAPTY_MONITOR
	       && child.type == S3K_CAPTY_MONITOR
	       && s3k_monitor_derive_monitor(parent.monitor, child.monitor);
}

bool s3k_channel_derive(union s3k_cap parent, union s3k_cap child)
{
	return (parent.type == S3K_CAPTY_CHANNEL
		&& child.type == S3K_CAPTY_CHANNEL
		&& s3k_channel_derive_channel(parent.channel, child.channel))
	       || (parent.type == S3K_CAPTY_CHANNEL
		   && child.type == S3K_CAPTY_SOCKET
		   && s3k_channel_derive_socket(parent.channel, child.socket));
}

bool s3k_socket_derive(union s3k_cap parent, union s3k_cap child)
{
	return parent.type == S3K_CAPTY_SOCKET && child.type == S3K_CAPTY_SOCKET
	       && s3k_socket_derive_socket(parent.socket, child.socket);
}

bool s3k_can_derive(union s3k_cap parent, union s3k_cap child)
{
	return s3k_time_derive(parent, child)
	       || s3k_memory_derive(parent, child)
	       || s3k_monitor_derive(parent, child)
	       || s3k_channel_derive(parent, child)
	       || s3k_socket_derive(parent, child);
}

bool s3k_time_parent_time(struct s3k_time parent, struct s3k_time child)
{
	return parent.begin <= child.begin && child.end <= parent.end
	       && child.hartid == parent.hartid;
}

static bool s3k_memory_parent_memory(struct s3k_memory parent,
				     struct s3k_memory child)
{
	return parent.offset == child.offset && parent.begin <= child.begin
	       && child.end <= parent.end;
}

static bool s3k_memory_parent_pmp(struct s3k_memory parent,
				  struct s3k_pmp child)
{
	uint64_t pmp_begin = s3k_pmp_napot_begin(child.addr);
	uint64_t pmp_end = s3k_pmp_napot_end(child.addr);
	uint64_t rwx = child.cfg & 0x7;
	uint64_t mem_free
	    = ((uint64_t)parent.offset << 27) + (parent.free << 12);
	uint64_t mem_end = ((uint64_t)parent.offset << 27) + (parent.end << 12);
	return mem_free <= pmp_begin && pmp_end <= mem_end
	       && ((parent.rwx & rwx) == rwx);
}

bool s3k_monitor_parent_monitor(struct s3k_monitor parent,
				struct s3k_monitor child)
{
	return parent.begin <= child.begin && child.end <= parent.end;
}

bool s3k_channel_parent_channel(struct s3k_channel parent,
				struct s3k_channel child)
{
	return parent.begin <= child.begin && child.end <= parent.end;
}

bool s3k_channel_parent_socket(struct s3k_channel parent,
			       struct s3k_socket child)
{
	return parent.begin <= child.channel && child.channel <= parent.end;
}

bool s3k_socket_parent_socket(struct s3k_socket parent, struct s3k_socket child)
{
	return parent.tag == 0 && parent.channel == child.channel;
}

bool s3k_time_parent(union s3k_cap parent, union s3k_cap child)
{
	return parent.type == S3K_CAPTY_TIME && child.type == S3K_CAPTY_TIME
	       && s3k_time_parent_time(parent.time, child.time);
}

bool s3k_memory_parent(union s3k_cap parent, union s3k_cap child)
{
	return (parent.type == S3K_CAPTY_MEMORY
		&& child.type == S3K_CAPTY_MEMORY
		&& s3k_memory_parent_memory(parent.memory, child.memory))
	       || (parent.type == S3K_CAPTY_MEMORY
		   && child.type == S3K_CAPTY_PMP
		   && s3k_memory_parent_pmp(parent.memory, child.pmp));
}

bool s3k_monitor_parent(union s3k_cap parent, union s3k_cap child)
{
	return parent.type == S3K_CAPTY_MONITOR
	       && child.type == S3K_CAPTY_MONITOR
	       && s3k_monitor_parent_monitor(parent.monitor, child.monitor);
}

bool s3k_channel_parent(union s3k_cap parent, union s3k_cap child)
{
	return (parent.type == S3K_CAPTY_CHANNEL
		&& child.type == S3K_CAPTY_CHANNEL
		&& s3k_channel_parent_channel(parent.channel, child.channel))
	       || (parent.type == S3K_CAPTY_CHANNEL
		   && child.type == S3K_CAPTY_SOCKET
		   && s3k_channel_parent_socket(parent.channel, child.socket));
}

bool s3k_socket_parent(union s3k_cap parent, union s3k_cap child)
{
	return parent.type == S3K_CAPTY_SOCKET && child.type == S3K_CAPTY_SOCKET
	       && s3k_socket_parent_socket(parent.socket, child.socket);
}

bool s3k_is_parent(union s3k_cap parent, union s3k_cap child)
{
	return s3k_time_parent(parent, child)
	       || s3k_memory_parent(parent, child)
	       || s3k_monitor_parent(parent, child)
	       || s3k_channel_parent(parent, child)
	       || s3k_socket_parent(parent, child);
}
