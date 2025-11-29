#pragma once

#include "csr.h"

#define CSR_MIP_MTIP (1 << 7)

/**
 * Check if the current hart should preempt.
 */
static inline bool preempt(void)
{
	return csrr_mip() & CSR_MIP_MTIP;
}
