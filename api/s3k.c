#include "s3k.h"

// Syscall
uint64_t s3k_getpid(void)
{
	register uint64_t a0 __asm__("a0") = S3K_SYSCALL_GETINFO;
	register uint64_t a1 __asm__("a1") = 0;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1));
	return a0;
}

uint64_t s3k_gethartid(void)
{
	register uint64_t a0 __asm__("a0") = S3K_SYSCALL_GETINFO;
	register uint64_t a1 __asm__("a1") = 1;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1));
	return a0;
}

uint64_t s3k_gettime(void)
{
	register uint64_t a0 __asm__("a0") = S3K_SYSCALL_GETINFO;
	register uint64_t a1 __asm__("a1") = 2;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1));
	return a0;
}

uint64_t s3k_gettimeout(void)
{
	register uint64_t a0 __asm__("a0") = S3K_SYSCALL_GETINFO;
	register uint64_t a1 __asm__("a1") = 3;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1));
	return a0;
}

uint64_t s3k_getreg(enum s3k_reg reg)
{
	register uint64_t a0 __asm__("a0") = S3K_SYSCALL_GETREG;
	register uint64_t a1 __asm__("a1") = reg;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1));
	return a0;
}

uint64_t s3k_setreg(enum s3k_reg reg, uint64_t val)
{
	register uint64_t a0 __asm__("a0") = S3K_SYSCALL_SETREG;
	register uint64_t a1 __asm__("a1") = reg;
	register uint64_t a2 __asm__("a2") = val;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2));
	return a0;
}

void s3k_yield(void)
{
	register uint64_t a0 __asm__("a0") = S3K_SYSCALL_YIELD;
	__asm__("ecall" : : "r"(a0));
}

union s3k_cap s3k_getcap(uint64_t i)
{
	register uint64_t a0 __asm__("a0") = S3K_SYSCALL_GETCAP;
	register uint64_t a1 __asm__("a1") = i;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1));
	return (union s3k_cap){ .raw = a0 };
}

enum s3k_excpt s3k_movcap(uint64_t i, uint64_t j)
{
	register uint64_t a0 __asm__("a0") = S3K_SYSCALL_MOVCAP;
	register uint64_t a1 __asm__("a1") = i;
	register uint64_t a2 __asm__("a2") = j;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2));
	return a0;
}

enum s3k_excpt s3k_delcap(uint64_t i)
{
	register uint64_t a0 __asm__("a0") = S3K_SYSCALL_DELCAP;
	register uint64_t a1 __asm__("a1") = i;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1));
	return a0;
}

enum s3k_excpt s3k_revcap(uint64_t i)
{
	register uint64_t a0 __asm__("a0") = S3K_SYSCALL_REVCAP;
	register uint64_t a1 __asm__("a1") = i;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1));
	return a0;
}

enum s3k_excpt s3k_drvcap(uint64_t i, uint64_t j, union s3k_cap cap)
{
	register uint64_t a0 __asm__("a0") = S3K_SYSCALL_DRVCAP;
	register uint64_t a1 __asm__("a1") = i;
	register uint64_t a2 __asm__("a2") = j;
	register uint64_t a3 __asm__("a3") = cap.raw;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2), "r"(a3));
	return a0;
}

enum s3k_excpt s3k_msuspend(uint64_t i, uint64_t pid)
{
	register uint64_t a0 __asm__("a0") = S3K_SYSCALL_MSUSPEND;
	register uint64_t a1 __asm__("a1") = i;
	register uint64_t a2 __asm__("a2") = pid;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2));
	return a0;
}

enum s3k_excpt s3k_mresume(uint64_t i, uint64_t pid)
{
	register uint64_t a0 __asm__("a0") = S3K_SYSCALL_MRESUME;
	register uint64_t a1 __asm__("a1") = i;
	register uint64_t a2 __asm__("a2") = pid;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2));
	return a0;
}

enum s3k_excpt s3k_mgetreg(uint64_t i, uint64_t pid, enum s3k_reg reg, uint64_t *val)
{
	register uint64_t a0 __asm__("a0") = S3K_SYSCALL_MGETREG;
	register uint64_t a1 __asm__("a1") = i;
	register uint64_t a2 __asm__("a2") = pid;
	register uint64_t a3 __asm__("a3") = reg;
	__asm__ volatile("ecall" : "+r"(a0), "+r"(a1) : "r"(a2), "r"(a3));
	if (a0 == S3K_EXCPT_NONE)
		*val = a1;
	return a0;
}

enum s3k_excpt s3k_msetreg(uint64_t i, uint64_t pid, enum s3k_reg reg, uint64_t val)
{
	register uint64_t a0 __asm__("a0") = S3K_SYSCALL_MSETREG;
	register uint64_t a1 __asm__("a1") = i;
	register uint64_t a2 __asm__("a2") = pid;
	register uint64_t a3 __asm__("a3") = reg;
	register uint64_t a4 __asm__("a4") = val;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2), "r"(a3), "r"(a4));
	return a0;
}

enum s3k_excpt s3k_mgetcap(uint64_t i, uint64_t pid, uint64_t j, union s3k_cap *cap)
{
	register uint64_t a0 __asm__("a0") = S3K_SYSCALL_MGETCAP;
	register uint64_t a1 __asm__("a1") = i;
	register uint64_t a2 __asm__("a2") = pid;
	register uint64_t a3 __asm__("a3") = j;
	__asm__ volatile("ecall" : "+r"(a0), "+r"(a1) : "r"(a2), "r"(a3));
	if (a0 == S3K_EXCPT_NONE)
		cap->raw = a1;
	return a0;
}

enum s3k_excpt s3k_mtakecap(uint64_t i, uint64_t pid, uint64_t src, uint64_t dst)
{
	register uint64_t a0 __asm__("a0") = S3K_SYSCALL_MTAKECAP;
	register uint64_t a1 __asm__("a1") = i;
	register uint64_t a2 __asm__("a2") = pid;
	register uint64_t a3 __asm__("a3") = src;
	register uint64_t a4 __asm__("a4") = dst;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2), "r"(a3), "r"(a4));
	return a0;
}

enum s3k_excpt s3k_mgivecap(uint64_t i, uint64_t pid, uint64_t src, uint64_t dst)
{
	register uint64_t a0 __asm__("a0") = S3K_SYSCALL_MGIVECAP;
	register uint64_t a1 __asm__("a1") = i;
	register uint64_t a2 __asm__("a2") = pid;
	register uint64_t a3 __asm__("a3") = src;
	register uint64_t a4 __asm__("a4") = dst;
	__asm__ volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2), "r"(a3), "r"(a4));
	return a0;
}

enum s3k_excpt s3k_recv(void);
enum s3k_excpt s3k_send(void);
enum s3k_excpt s3k_sendrecv(void);

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

union s3k_cap s3k_time(uint64_t hartid, uint64_t begin, uint64_t end)
{
	return (union s3k_cap){ .time = { S3K_CAPTY_TIME, 0, hartid, begin, begin, end } };
}

union s3k_cap s3k_memory(uint64_t begin, uint64_t end, uint64_t offset, uint64_t rwx)
{
	return (union s3k_cap){ .memory = { S3K_CAPTY_MEMORY, false, rwx, offset, begin, begin, end } };
}

union s3k_cap s3k_pmp(uint64_t addr, uint64_t rwx)
{
	return (union s3k_cap){ .pmp = { S3K_CAPTY_PMP, addr, 0x18 | rwx } };
}

union s3k_cap s3k_monitor(uint64_t begin, uint64_t end)
{
	return (union s3k_cap){ .monitor = { S3K_CAPTY_MONITOR, 0, begin, begin, end } };
}

union s3k_cap s3k_channel(uint64_t begin, uint64_t end)
{
	return (union s3k_cap){ .channel = { S3K_CAPTY_CHANNEL, 0, begin, begin, end } };
}

union s3k_cap s3k_socket(uint64_t channel, uint64_t tag)
{
	return (union s3k_cap){ .socket = { S3K_CAPTY_SOCKET, 0, channel, tag } };
}

bool s3k_time_derive_time(struct s3k_time parent, struct s3k_time child)
{
	return parent.free <= child.begin && child.end <= parent.end && child.begin == child.free
	       && child.begin < child.end && child._padd == 0;
}

bool s3k_memory_derive_memory(struct s3k_memory parent, struct s3k_memory child)
{
	return parent.free <= child.begin && child.end <= parent.end && child.begin == child.free
	       && child.begin < child.end && (parent.rwx & child.rwx) == child.rwx && parent.lock == 0;
}

static bool s3k_memory_derive_pmp(struct s3k_memory parent, struct s3k_pmp child)
{
	uint64_t pmp_begin = pmp_napot_begin(child.addr);
	uint64_t pmp_end = pmp_napot_end(child.addr);
	uint64_t rwx = child.cfg & 0x7;
	uint64_t mode = child.cfg >> 3;
	uint64_t mem_free = ((uint64_t)parent.offset << 27) + (parent.free << 12);
	uint64_t mem_end = ((uint64_t)parent.offset << 27) + (parent.end << 12);
	return mem_free <= pmp_begin && pmp_end <= mem_end && ((parent.rwx & rwx) == rwx) && mode == 0x3;
}

bool s3k_monitor_derive_monitor(struct s3k_monitor parent, struct s3k_monitor child)
{
	return parent.free <= child.begin && child.end <= parent.end && child.begin == child.free
	       && child.begin < child.end;
}

bool s3k_channel_derive_channel(struct s3k_channel parent, struct s3k_channel child)
{
	return parent.free <= child.begin && child.end <= parent.end && child.begin == child.free
	       && child.begin < child.end && child._padd == 0;
}

bool s3k_channel_derive_socket(struct s3k_channel parent, struct s3k_socket child)
{
	return parent.free <= child.channel && child.channel < parent.end && child.tag == 0 && child._padd == 0;
}

bool s3k_socket_derive_socket(struct s3k_socket parent, struct s3k_socket child)
{
	return parent.channel == child.channel && parent.tag == 0 && child.tag > 0 && child._padd == 0;
}

bool s3k_time_derive(union s3k_cap parent, union s3k_cap child)
{
	return parent.type == S3K_CAPTY_TIME && child.type == S3K_CAPTY_TIME
	       && s3k_time_derive_time(parent.time, child.time);
}

bool s3k_memory_derive(union s3k_cap parent, union s3k_cap child)
{
	return (parent.type == S3K_CAPTY_MEMORY && child.type == S3K_CAPTY_MEMORY
		&& s3k_memory_derive_memory(parent.memory, child.memory))
	       || (parent.type == S3K_CAPTY_MEMORY && child.type == S3K_CAPTY_PMP
		   && s3k_memory_derive_pmp(parent.memory, child.pmp));
}

bool s3k_monitor_derive(union s3k_cap parent, union s3k_cap child)
{
	return parent.type == S3K_CAPTY_MONITOR && child.type == S3K_CAPTY_MONITOR
	       && s3k_monitor_derive_monitor(parent.monitor, child.monitor);
}

bool s3k_channel_derive(union s3k_cap parent, union s3k_cap child)
{
	return (parent.type == S3K_CAPTY_CHANNEL && child.type == S3K_CAPTY_CHANNEL
		&& s3k_channel_derive_channel(parent.channel, child.channel))
	       || (parent.type == S3K_CAPTY_CHANNEL && child.type == S3K_CAPTY_SOCKET
		   && s3k_channel_derive_socket(parent.channel, child.socket));
}

bool s3k_socket_derive(union s3k_cap parent, union s3k_cap child)
{
	return parent.type == S3K_CAPTY_SOCKET && child.type == S3K_CAPTY_SOCKET
	       && s3k_socket_derive_socket(parent.socket, child.socket);
}

bool s3k_can_derive(union s3k_cap parent, union s3k_cap child)
{
	return s3k_time_derive(parent, child) || s3k_memory_derive(parent, child) || s3k_monitor_derive(parent, child)
	       || s3k_channel_derive(parent, child) || s3k_socket_derive(parent, child);
}

bool s3k_time_parent_time(struct s3k_time parent, struct s3k_time child)
{
	return parent.begin <= child.begin && child.end <= parent.free;
}

static bool s3k_memory_parent_memory(struct s3k_memory parent, struct s3k_memory child)
{
	return parent.offset == child.offset && parent.begin <= child.begin && child.end <= parent.free;
}

static bool s3k_memory_parent_pmp(struct s3k_memory parent, struct s3k_pmp child)
{
	uint64_t pmp_begin = pmp_napot_begin(child.addr);
	uint64_t pmp_end = pmp_napot_end(child.addr);
	uint64_t rwx = child.cfg & 0x7;
	uint64_t mem_free = ((uint64_t)parent.offset << 27) + (parent.free << 12);
	uint64_t mem_end = ((uint64_t)parent.offset << 27) + (parent.end << 12);
	return mem_free <= pmp_begin && pmp_end <= mem_end && ((parent.rwx & rwx) == rwx);
}

bool s3k_monitor_parent_monitor(struct s3k_monitor parent, struct s3k_monitor child)
{
	return parent.begin <= child.begin && child.end <= parent.free;
}

bool s3k_channel_parent_channel(struct s3k_channel parent, struct s3k_channel child)
{
	return parent.begin <= child.begin && child.end <= parent.free;
}

bool s3k_channel_parent_socket(struct s3k_channel parent, struct s3k_socket child)
{
	return parent.begin <= child.channel && child.channel <= parent.free;
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
	return (parent.type == S3K_CAPTY_MEMORY && child.type == S3K_CAPTY_MEMORY
		&& s3k_memory_parent_memory(parent.memory, child.memory))
	       || (parent.type == S3K_CAPTY_MEMORY && child.type == S3K_CAPTY_PMP
		   && s3k_memory_parent_pmp(parent.memory, child.pmp));
}

bool s3k_monitor_parent(union s3k_cap parent, union s3k_cap child)
{
	return parent.type == S3K_CAPTY_MONITOR && child.type == S3K_CAPTY_MONITOR
	       && s3k_monitor_parent_monitor(parent.monitor, child.monitor);
}

bool s3k_channel_parent(union s3k_cap parent, union s3k_cap child)
{
	return (parent.type == S3K_CAPTY_CHANNEL && child.type == S3K_CAPTY_CHANNEL
		&& s3k_channel_parent_channel(parent.channel, child.channel))
	       || (parent.type == S3K_CAPTY_CHANNEL && child.type == S3K_CAPTY_SOCKET
		   && s3k_channel_parent_socket(parent.channel, child.socket));
}

bool s3k_socket_parent(union s3k_cap parent, union s3k_cap child)
{
	return parent.type == S3K_CAPTY_SOCKET && child.type == S3K_CAPTY_SOCKET
	       && s3k_socket_parent_socket(parent.socket, child.socket);
}

bool s3k_is_parent(union s3k_cap parent, union s3k_cap child)
{
	return s3k_time_parent(parent, child) || s3k_memory_parent(parent, child) || s3k_monitor_parent(parent, child)
	       || s3k_channel_parent(parent, child) || s3k_socket_parent(parent, child);
}
