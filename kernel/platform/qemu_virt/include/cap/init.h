#pragma once
#define INIT_CAPS                                                 \
	{                                                         \
	    [0] = cap_mk_pmp(0x20005fff, MEM_RWX),                \
	    [1] = cap_mk_memory(0x80020000, 0x88000000, MEM_RWX), \
	    [2] = cap_mk_memory(0x10000000, 0x10001000, MEM_RW),  \
	    [3] = cap_mk_memory(0x200b000, 0x200c000, MEM_R),     \
	    [4] = cap_mk_time(0, 0, NSLOT),                       \
	    [8] = cap_mk_monitor(0, NPROC),                       \
	    [9] = cap_mk_channel(0, NCHAN),                       \
	}
