#pragma once

#include "kern/cap.h"
#include "kern/csr.h"
#include "kern/types.h"

static inline bool BitSubset(Word a, Word b)
{
	return (a & ~b) == 0;
}

static inline Word PmpNapotDecodeBase(Word addr)
{
	return ((addr + 1) & addr) << 2;
}

static inline Word PmpNapotDecodeSize(Word addr)
{
	return (((addr + 1) ^ addr) + 1) << 2;
}

static inline Word CapMemBegin(Cap cap)
{
	return (cap.mem.tag << 27) + (cap.mem.begin << 12);
}

static inline Word CapMemEnd(Cap cap)
{
	return (cap.mem.tag << 27) + (cap.mem.end << 12);
}

static inline Word CapMemMark(Cap cap)
{
	return (cap.mem.tag << 27) + (cap.mem.mark << 12);
}

static inline Word CapPmpBegin(Cap cap)
{
	return PmpNapotDecodeBase(cap.pmp.addr);
}

static inline Word CapPmpEnd(Cap cap)
{
	return PmpNapotDecodeBase(cap.pmp.addr) + PmpNapotDecodeSize(cap.pmp.addr);
}

static inline Word Preempted(void)
{
	return CsrRead(mip) & CsrRead(mie);
}
