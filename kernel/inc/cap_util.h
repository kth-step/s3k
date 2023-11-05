#pragma once
#include "cap_types.h"

cap_t cap_mk_time(hart_t hart, time_slot_t bgn, time_slot_t end);
cap_t cap_mk_memory(addr_t bgn, addr_t end, rwx_t rwx);
cap_t cap_mk_pmp(napot_t addr, rwx_t rwx);
cap_t cap_mk_monitor(pid_t bgn, pid_t end);
cap_t cap_mk_channel(chan_t bgn, chan_t end);
cap_t cap_mk_socket(chan_t chan, ipc_mode_t mode, ipc_perm_t perm,
		    uint32_t tag);

static inline addr_t tag_block_to_addr(tag_t tag, block_t block)
{
	return ((uint64_t)tag << MAX_BLOCK_SIZE)
	       + ((uint64_t)block << MIN_BLOCK_SIZE);
}

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

bool cap_is_valid(cap_t cap);
bool cap_is_revokable(cap_t parent, cap_t child);
bool cap_is_derivable(cap_t parent, cap_t child);
