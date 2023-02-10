#include "csr.h"

#define CSRR(X)                                              \
	uint64_t csrr_##X(void)                              \
	{                                                    \
		uint64_t val;                                \
		__asm__ volatile("csrr %0," #X : "=r"(val)); \
		return val;                                  \
	}

#define CSRW(X)                                                \
	void csrw_##X(uint64_t val)                            \
	{                                                      \
		__asm__ volatile("csrw " #X ",%0" ::"r"(val)); \
	}

#define CSRS(X)                                                \
	void csrs_##X(uint64_t val)                            \
	{                                                      \
		__asm__ volatile("csrs " #X ",%0" ::"r"(val)); \
	}

CSRR(mhartid)
CSRR(mip)

CSRR(pmpcfg0)
CSRR(pmpaddr0)
CSRR(pmpaddr1)
CSRR(pmpaddr2)
CSRR(pmpaddr3)
CSRR(pmpaddr4)
CSRR(pmpaddr5)
CSRR(pmpaddr6)
CSRR(pmpaddr7)

CSRW(pmpcfg0)
CSRS(pmpcfg0)
CSRW(pmpaddr0)
CSRW(pmpaddr1)
CSRW(pmpaddr2)
CSRW(pmpaddr3)
CSRW(pmpaddr4)
CSRW(pmpaddr5)
CSRW(pmpaddr6)
CSRW(pmpaddr7)
