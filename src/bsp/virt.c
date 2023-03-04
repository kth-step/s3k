#include "platform.h"

#define MTIME ((volatile uint64_t *)0x200bff8ull)
#define MTIMECMP(x) ((volatile uint64_t *)(0x2004000ull + ((x)*8ull)))

uint64_t time_get(void)
{
	return *MTIME;
}

uint64_t timeout_get(uint64_t i)
{
	return *MTIMECMP(i);
}

void timeout_set(uint64_t i, uint64_t val)
{
	*MTIMECMP(i) = val;
}

uint64_t csrr_mhartid(void)
{
	register uint64_t val;
	__asm__("csrr %0,mhartid" : "=r"(val));
	return val;
}

uint64_t csrr_mip(void)
{
	register uint64_t val;
	__asm__ volatile ("csrr %0,mip" : "=r"(val));
	return val;
}

uint64_t csrr_pmpcfg(uint64_t i)
{
	register uint64_t val;
	__asm__ volatile("csrr %0,pmpcfg0" : "=r"(val));
	return val;
}

void csrw_pmpcfg(uint64_t i, uint64_t val)
{
	__asm__ volatile("csrw pmpcfg0,%0" ::"r"(val));
}

uint64_t csrr_pmpaddr(uint64_t i)
{
	register uint64_t val;
	switch (i) {
	case 0:
		__asm__ volatile("csrr %0,pmpaddr0" : "=r"(val));
		break;
	case 1:
		__asm__ volatile("csrr %0,pmpaddr1" : "=r"(val));
		break;
	case 2:
		__asm__ volatile("csrr %0,pmpaddr2" : "=r"(val));
		break;
	case 3:
		__asm__ volatile("csrr %0,pmpaddr3" : "=r"(val));
		break;
	case 4:
		__asm__ volatile("csrr %0,pmpaddr4" : "=r"(val));
		break;
	case 5:
		__asm__ volatile("csrr %0,pmpaddr5" : "=r"(val));
		break;
	case 6:
		__asm__ volatile("csrr %0,pmpaddr6" : "=r"(val));
		break;
	case 7:
		__asm__ volatile("csrr %0,pmpaddr7" : "=r"(val));
		break;
	}
	return val;
}

void csrw_pmpaddr(uint64_t i, uint64_t val)
{
	switch (i) {
	case 0:
		__asm__ volatile("csrw pmpaddr0,%0" ::"r"(val));
		break;
	case 1:
		__asm__ volatile("csrw pmpaddr1,%0" ::"r"(val));
		break;
	case 2:
		__asm__ volatile("csrw pmpaddr2,%0" ::"r"(val));
		break;
	case 3:
		__asm__ volatile("csrw pmpaddr3,%0" ::"r"(val));
		break;
	case 4:
		__asm__ volatile("csrw pmpaddr4,%0" ::"r"(val));
		break;
	case 5:
		__asm__ volatile("csrw pmpaddr5,%0" ::"r"(val));
		break;
	case 6:
		__asm__ volatile("csrw pmpaddr6,%0" ::"r"(val));
		break;
	case 7:
		__asm__ volatile("csrw pmpaddr7,%0" ::"r"(val));
		break;
	default:
		__builtin_unreachable();
	}
}
