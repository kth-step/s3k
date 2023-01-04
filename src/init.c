/* See LICENSE file for copyright and license details. */
#include "init.h"

#include "cap_node.h"
#include "common.h"
#include "init_caps.h"
#include "proc.h"

void init_kernel(uint64_t payload)
{
	for (uint64_t pid = 0; pid < NPROC; ++pid)
		processes[pid].pid = pid;
	init_caps(&processes[0]);
	processes[0].regs.pc = payload;
}
