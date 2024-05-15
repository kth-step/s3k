#pragma once
#include "cap_types.h"

cap_t cap_mk_time(hart_t hart, time_slot_t bgn, time_slot_t end);
cap_t cap_mk_memory(addr_t bgn, addr_t end, rwx_t rwx);
cap_t cap_mk_pmp(napot_t addr, rwx_t rwx);
cap_t cap_mk_monitor(pid_t bgn, pid_t end);
cap_t cap_mk_channel(chan_t bgn, chan_t end);
cap_t cap_mk_socket(chan_t chan, ipc_mode_t mode, ipc_perm_t perm,
		    uint32_t tag);

bool cap_is_valid(cap_t cap);
bool cap_is_revokable(cap_t parent, cap_t child);
bool cap_is_derivable(cap_t parent, cap_t child);
