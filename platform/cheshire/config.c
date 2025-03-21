#include "cap/types.h"
#include "cap/util.h"

cap_t cap_init(int i)
{
	cap_t init_caps[] = {
	    [0] = cap_mk_pmp(0x20005fff, MEM_RWX),
	    [1] = cap_mk_memory(0x80020000, 0x88000000, MEM_RWX),
	    [2] = cap_mk_memory(0x03002000, 0x03003000, MEM_RW),
	    [3] = cap_mk_memory(0x204b000, 0x204c000, MEM_R),
	    [4] = cap_mk_time(0, 0, NSLOT),
	    [5] = cap_mk_monitor(0, NPROC),
	    [6] = cap_mk_channel(0, NCHAN),
	    [7] = {.raw = 0},
	};

	return init_caps[i];
}
