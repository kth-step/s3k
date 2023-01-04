/* See LICENSE file for copyright and license details. */
#include "cap.h"
#include "cap_node.h"
#include "init.h"
#include "proc.h"

/* Add the initial capabilities for the process */
void init_caps(struct proc *proc)
{
	static struct cap_node sentinel = {0};
	sentinel.prev = &sentinel;
	sentinel.next = &sentinel;
	struct cap_node *node = proc->caps;

	/* Init process's pmp slice */
	/* 0x20005fff == (0x80010000, 0x80020000) */
	node->cap = cap_pmp(0x20005fff, 0x7);
	cap_node_insert(node++, &sentinel);

	/* User memory */
	/* (0x80020000, 0x100000000) */
	node->cap = cap_memory(0x80020, 0x80020, 0x100000, 0x7, 0);
	cap_node_insert(node++, &sentinel);

	/* Add time slices */
	node->cap = cap_time(0, 0, 0, NSLICE);
	cap_node_insert(node++, &sentinel);
	node->cap = cap_time(1, 0, 0, NSLICE);
	cap_node_insert(node++, &sentinel);
	node->cap = cap_time(2, 0, 0, NSLICE);
	cap_node_insert(node++, &sentinel);
	node->cap = cap_time(3, 0, 0, NSLICE);
	cap_node_insert(node++, &sentinel);

	/* Monitor capability */
	node->cap = cap_monitor(0, 0, NPROC);
	cap_node_insert(node++, &sentinel);
}
