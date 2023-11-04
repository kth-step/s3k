#include "csr.h"

uint64_t csrr_mcause(void)
{
	uint64_t val;
	__asm__ volatile("csrr %0,mcause" : "=r"(val));
	return val;
}

uint64_t csrr_mtval(void)
{
	uint64_t val;
	__asm__ volatile("csrr %0,mtval" : "=r"(val));
	return val;
}

uint64_t csrr_mepc(void)
{
	uint64_t val;
	__asm__ volatile("csrr %0,mepc" : "=r"(val));
	return val;
}

uint64_t csrr_mhartid(void)
{
	uint64_t val;
	__asm__ volatile("csrr %0,mhartid" : "=r"(val));
	return val;
}

uint64_t csrr_mip(void)
{
	uint64_t val;
	__asm__ volatile("csrr %0,mip" : "=r"(val));
	return val;
}

uint64_t csrr_mcycle(void)
{
	uint64_t val;
	__asm__ volatile("csrr %0,mcycle" : "=r"(val));
	return val;
}

void csrw_mcycle(uint64_t val)
{
	__asm__ volatile("csrw mcycle,%0" ::"r"(val));
}

uint64_t csrr_mhpmcounter3(void)
{
	uint64_t val;
	__asm__ volatile("csrr %0,mhpmcounter3" : "=r"(val));
	return val;
}

void csrw_mhpmcounter3(uint64_t val)
{
	__asm__ volatile("csrw mhpmcounter3,%0" ::"r"(val));
}

void csrw_mstatus(uint64_t val)
{
	__asm__ volatile("csrw mstatus,%0" ::"r"(val));
}

void csrs_mstatus(uint64_t val)
{
	__asm__ volatile("csrs mstatus,%0" ::"r"(val));
}

void csrc_mstatus(uint64_t val)
{
	__asm__ volatile("csrc mstatus,%0" ::"r"(val));
}

uint64_t csrr_pmpcfg0(void)
{
	uint64_t val;
	__asm__ volatile("csrr %0,pmpcfg0" : "=r"(val));
	return val;
}

void csrw_pmpcfg0(uint64_t val)
{
	__asm__ volatile("csrw pmpcfg0,%0" ::"r"(val));
}

uint64_t csrr_pmpaddr0(void)
{
	uint64_t val;
	__asm__ volatile("csrr %0,pmpaddr0" : "=r"(val));
	return val;
}

uint64_t csrr_pmpaddr1(void)
{
	uint64_t val;
	__asm__ volatile("csrr %0,pmpaddr1" : "=r"(val));
	return val;
}

uint64_t csrr_pmpaddr2(void)
{
	uint64_t val;
	__asm__ volatile("csrr %0,pmpaddr2" : "=r"(val));
	return val;
}

uint64_t csrr_pmpaddr3(void)
{
	uint64_t val;
	__asm__ volatile("csrr %0,pmpaddr3" : "=r"(val));
	return val;
}

uint64_t csrr_pmpaddr4(void)
{
	uint64_t val;
	__asm__ volatile("csrr %0,pmpaddr4" : "=r"(val));
	return val;
}

uint64_t csrr_pmpaddr5(void)
{
	uint64_t val;
	__asm__ volatile("csrr %0,pmpaddr5" : "=r"(val));
	return val;
}

uint64_t csrr_pmpaddr6(void)
{
	uint64_t val;
	__asm__ volatile("csrr %0,pmpaddr6" : "=r"(val));
	return val;
}

uint64_t csrr_pmpaddr7(void)
{
	uint64_t val;
	__asm__ volatile("csrr %0,pmpaddr7" : "=r"(val));
	return val;
}

void csrw_pmpaddr0(uint64_t val)
{
	__asm__ volatile("csrw pmpaddr0,%0" ::"r"(val));
}

void csrw_pmpaddr1(uint64_t val)
{
	__asm__ volatile("csrw pmpaddr1,%0" ::"r"(val));
}

void csrw_pmpaddr2(uint64_t val)
{
	__asm__ volatile("csrw pmpaddr2,%0" ::"r"(val));
}

void csrw_pmpaddr3(uint64_t val)
{
	__asm__ volatile("csrw pmpaddr3,%0" ::"r"(val));
}

void csrw_pmpaddr4(uint64_t val)
{
	__asm__ volatile("csrw pmpaddr4,%0" ::"r"(val));
}

void csrw_pmpaddr5(uint64_t val)
{
	__asm__ volatile("csrw pmpaddr5,%0" ::"r"(val));
}

void csrw_pmpaddr6(uint64_t val)
{
	__asm__ volatile("csrw pmpaddr6,%0" ::"r"(val));
}

void csrw_pmpaddr7(uint64_t val)
{
	__asm__ volatile("csrw pmpaddr7,%0" ::"r"(val));
}
