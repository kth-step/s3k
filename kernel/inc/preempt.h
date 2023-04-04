#pragma once
#include "csr.h"

#include <stdbool.h>

static inline bool preempt(void)
{
	return csrr_mip() & MIP_MTIP;
}

static inline void preempt_enable(void)
{
	csrs_mstatus(MSTATUS_MIE);
}

static inline void preempt_disable(void)
{
	csrc_mstatus(MSTATUS_MIE);
}
