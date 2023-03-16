#include "cap.h"

static const union cap init_caps[]
    = { [0] = CAP_PMP(0x20005fff, CAP_RWX),
	[1] = CAP_MEMORY(0x0020, 0x8000, 0x10, CAP_RWX),
	[2] = CAP_MEMORY(0x0000, 0x0001, 0x2, CAP_RW),
	[3] = CAP_TIME(0, 0, NSLICE),
	[4] = CAP_TIME(1, 0, NSLICE),
	[5] = CAP_TIME(2, 0, NSLICE),
	[6] = CAP_TIME(3, 0, NSLICE),
	[7] = CAP_MONITOR(0, NPROC),
	[8] = CAP_CHANNEL(0, NCHANNEL) };
