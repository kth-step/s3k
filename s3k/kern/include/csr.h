#ifndef CSR_H
#define CSR_H

#include "types.h"

static inline uint64_t csrr_mcycle(void)
{
	uint64_t val;
	__asm__ volatile("csrr %0, mcycle" : "=r"(val));
	return val;
}

static inline void csrw_mcycle(uint64_t val)
{
	__asm__ volatile("csrw mcycle,%0" ::"r"(val));
}

static inline word_t csrr_mip(void)
{
	word_t val;
	__asm__ volatile("csrr %0, mip" : "=r"(val));
	return val;
}

static inline word_t csrr_mhartid(void)
{
	word_t val;
	__asm__ volatile("csrr %0, mhartid" : "=r"(val));
	return val;
}

#endif // CSR_H
