// See LICENSE file for copyright and license details.
#include "macro.S"
.globl _start

.extern init_kernel
.extern sched_next
.extern trap_entry
.extern trap_exit

.section .init

_start:
	.option push
	.option norelax
	la      gp, __global_pointer$
	.option pop
	load_sp t0

	// zero bss section
	la t0, _bss
	la t1, _end
	j  2f

1:
	sd   zero, (t0)
	addi t0, t0, 8

2:
	bne t0, t1, 1b

	// set trap entry
	la   t0, trap_entry
	csrw mtvec, t0

	csrw pmpcfg0, 0x0F
	li   t0, -1
	csrw pmpaddr0, t0

	csrw mstatus, 0
	li   t0, (1<<7)
	csrw mie, t0

	// initialize the kernel
	la a0,_payload
	call init_kernel
	call schedule_next
	tail trap_exit
