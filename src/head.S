// See LICENSE file for copyright and license details.
#include "stack.h"
#include "macro.inc"
.globl _start
.globl stack_top

.extern init_kernel
.extern sched_next
.extern trap_entry
.extern trap_exit

.section .text.init

_start:
	// Load the global and stack pointers.
	.option push
	.option norelax
	la      gp, __global_pointer$
	.option pop
	load_sp t0



	// Set trap entry/handler.
	la   t0, trap_entry
	csrw mtvec, t0

	// Disable interrupts in machine mode.
	csrw mstatus, 0
	// Enable timer interrupts for user mode.
	li   t0, (1<<7)
	csrw mie, t0

	// write zeros to be bss section
	la t0, _bss
	la t1, _end
	j  2f
1: 	sd zero, (t0)
	addi t0, t0, 8
2: 	bne t0, t1, 1b

	// Initialize the kernel.
	la a0,__payload
	call init_kernel
	// Schedule the boot process.
	call schedule_next
	// Load the boot process.
	tail trap_exit

.section .bss.stack
.balign 8
stack_start:
	.space (STACK_SIZE * NHART)
stack_top:
