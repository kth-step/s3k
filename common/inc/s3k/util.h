#pragma once
#include "s3k/types.h"

s3k_cap_t s3k_mk_time(s3k_hart_t hart, s3k_time_slot_t bgn,
		      s3k_time_slot_t end);
s3k_cap_t s3k_mk_memory(s3k_addr_t bgn, s3k_addr_t end, s3k_rwx_t rwx);
s3k_cap_t s3k_mk_pmp(s3k_napot_t napot_addr, s3k_rwx_t rwx);
s3k_cap_t s3k_mk_monitor(s3k_pid_t bgn, s3k_pid_t end);
s3k_cap_t s3k_mk_channel(s3k_chan_t bgn, s3k_chan_t end);
s3k_cap_t s3k_mk_socket(s3k_chan_t chan, s3k_ipc_mode_t mode,
			s3k_ipc_perm_t perm, uint32_t tag);

bool s3k_is_valid(s3k_cap_t a);
bool s3k_is_parent(s3k_cap_t a, s3k_cap_t b);
bool s3k_is_derivable(s3k_cap_t a, s3k_cap_t b);

void s3k_napot_decode(s3k_napot_t napot_addr, s3k_addr_t *begin,
		      s3k_addr_t *end);
s3k_napot_t s3k_napot_encode(s3k_addr_t base, s3k_addr_t size);

static inline bool s3k_is_ready(s3k_state_t state)
{
	return state == 0;
}

static inline bool s3k_is_busy(s3k_state_t state)
{
	return state & S3K_PSF_BUSY;
}

static inline bool s3k_is_blocked(s3k_state_t state, s3k_chan_t *chan)
{
	*chan = state >> 32;
	return state & S3K_PSF_BLOCKED;
}

static inline bool s3k_is_suspended(s3k_state_t state)
{
	return state == S3K_PSF_SUSPENDED;
}
