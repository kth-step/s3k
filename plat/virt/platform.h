#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#define NHART 4

#ifndef __ASSEMBLER__
#define INITIAL_CAPABILITIES                                                \
	{                                                                   \
		[0] = cap_pmp(0x20005fff, CAP_RWX),                         \
		[1] = cap_memory(0x0020, 0x8000, 0x10, CAP_RWX),            \
		[2] = cap_memory(0x0000, 0x0001, 0x2, CAP_RW),              \
		[3] = cap_time(0, 0, NSLICE), [4] = cap_time(1, 0, NSLICE), \
		[5] = cap_time(2, 0, NSLICE), [6] = cap_time(3, 0, NSLICE), \
		[7] = cap_monitor(0, NPROC), [8] = cap_channel(0, NCHANNEL) \
	}
#endif /* __ASSEMBLER__ */

#endif /* __PLATFORM_H__ */
