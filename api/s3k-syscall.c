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

enum s3k_excpt s3k_mgetreg(uint64_t i, uint64_t pid, enum s3k_reg reg,
			   uint64_t *val)
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

enum s3k_excpt s3k_msetreg(uint64_t i, uint64_t pid, enum s3k_reg reg,
			   uint64_t val)
{
	register uint64_t a0 __asm__("a0") = S3K_SYSCALL_MSETREG;
	register uint64_t a1 __asm__("a1") = i;
	register uint64_t a2 __asm__("a2") = pid;
	register uint64_t a3 __asm__("a3") = reg;
	register uint64_t a4 __asm__("a4") = val;
	__asm__ volatile("ecall"
			 : "+r"(a0)
			 : "r"(a1), "r"(a2), "r"(a3), "r"(a4));
	return a0;
}

enum s3k_excpt s3k_mgetcap(uint64_t i, uint64_t pid, uint64_t j,
			   union s3k_cap *cap)
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

enum s3k_excpt s3k_mtakecap(uint64_t i, uint64_t pid, uint64_t src,
			    uint64_t dst)
{
	register uint64_t a0 __asm__("a0") = S3K_SYSCALL_MTAKECAP;
	register uint64_t a1 __asm__("a1") = i;
	register uint64_t a2 __asm__("a2") = pid;
	register uint64_t a3 __asm__("a3") = src;
	register uint64_t a4 __asm__("a4") = dst;
	__asm__ volatile("ecall"
			 : "+r"(a0)
			 : "r"(a1), "r"(a2), "r"(a3), "r"(a4));
	return a0;
}

enum s3k_excpt s3k_mgivecap(uint64_t i, uint64_t pid, uint64_t src,
			    uint64_t dst)
{
	register uint64_t a0 __asm__("a0") = S3K_SYSCALL_MGIVECAP;
	register uint64_t a1 __asm__("a1") = i;
	register uint64_t a2 __asm__("a2") = pid;
	register uint64_t a3 __asm__("a3") = src;
	register uint64_t a4 __asm__("a4") = dst;
	__asm__ volatile("ecall"
			 : "+r"(a0)
			 : "r"(a1), "r"(a2), "r"(a3), "r"(a4));
	return a0;
}

enum s3k_excpt s3k_recv(void);
enum s3k_excpt s3k_send(void);
enum s3k_excpt s3k_sendrecv(void);
