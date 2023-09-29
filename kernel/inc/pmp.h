#pragma once

#include "proc.h"

#include <stdint.h>

static inline void pmp_napot_decode(uint64_t addr, uint64_t *base,
				    uint64_t *size)
{
	*base = ((addr + 1) & addr) << 2;
	*size = (((addr + 1) ^ addr) + 1) << 2;
}

static inline uint64_t pmp_napot_encode(uint64_t base, uint64_t size)
{
	return (base | (size / 2 - 1)) >> 2;
}
