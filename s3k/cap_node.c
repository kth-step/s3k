// See LICENSE file for copyright and license details.
#include "cap_node.h"

#include "preemption.h"
#include "proc.h"
#include "sched.h"

/** Capability table */
cap_node_t cap_tables[N_PROC][N_CAPS];
