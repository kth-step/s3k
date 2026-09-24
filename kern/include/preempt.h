#pragma once

#include "csr.h"

#define CSR_MIP_MTIP (1 << 7)

#ifdef PLATFORM_PREEMPT_STK
/* QingKe V4F: no MTIP and COUNTIF cannot be cleared, so preemption is driven
 * by the STK shadow clock (platform file). */
bool platform_timer_pending(void);
#define PREEMPT_PENDING() platform_timer_pending()
#endif

/* Platforms whose timer interrupt does not reach mip.MTIP may override
 * PREEMPT_PENDING() with their own poll. */
#ifndef PREEMPT_PENDING
#define PREEMPT_PENDING() (csrr_mip() & CSR_MIP_MTIP)
#endif

static inline bool preempt(void)
{
	return PREEMPT_PENDING();
}
