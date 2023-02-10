/* See LICENSE file for copyright and license details. */
#include "cap.h"
#include "cnode.h"
#include "common.h"
#include "init.h"

/* Add the initial capabilities for the process */
void init_caps(void)
{
	// All caps should be distinct
	const union cap caps[] = {
	    cap_pmp(0x20003fff, CAP_RWX),	       // RWX to init proc program
	    cap_memory(0x0020, 0xFFFF, 0x8, CAP_RWX),  // Main memory capability
	    cap_memory(0x0000, 0x0001, 0x1, CAP_RW),   // UART memory capability
	    cap_time(0, 0, NSLICE),		       // time to hart 0
	    cap_time(1, 0, NSLICE),		       // time to hart 1
	    cap_time(2, 0, NSLICE),		       // time to hart 2
	    cap_time(3, 0, NSLICE),		       // time to hart 3
	    cap_monitor(0, NPROC),		       // Monitor capability to all processes
	    cap_channel(0, NCHANNEL),		       // CHannel capability
	};
	// Initialize cnode to get working root handle
	cnode_init();
	// Ger root handle
	cnode_handle_t root = cnode_get_root_handle();
	// Add capabilities to proc 0
	for (size_t i = 0; i < ARRAY_SIZE(caps); i++) {
		uint32_t handle = cnode_get_handle(0, i);
		cnode_insert(handle, caps[i], root);
	}
}
