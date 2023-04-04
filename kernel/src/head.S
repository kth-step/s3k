// See LICENSE file for copyright and license details.
#include "macro.inc"
#include "offsets.h"
#include "csr.h"

.extern init_kernel
.extern sched
.extern trap_entry
.extern trap_exit

.section .text.init,"ax",@progbits

.globl _start
.type _start, @function
_start:
	/* Load gloabl pointer and stack pointer */
	load_gp
	load_sp t1

	/* Set some CSRs to 0 */
	csrw mstatus, 0
	csrw medeleg, 0
	csrw mideleg, 0
	csrw mscratch, 0
	csrw mie, 0
	csrw satp, 0

	/* Set trap entry. */
	la t0, trap_entry
	csrw mtvec, t0

	/* If hartid != MIN_HARTID, then jump to wait. */
	csrr t0, mhartid
	li t1, MIN_HARTID
	bne t0, t1, wait

zero_bss: /* write zeros to the bss section */
	la t0, _bss
	la t1, _end
	j  2f
1:	sb zero, (t0)
	addi t0, t0, 1
2:	bne t0, t1, 1b

	call kernel_init

wake:   /* Wake harts MIN_HARTID .. MAX_HARTID. */
	/* Wake using software interrupt */
	la t0, _clint
	addi t1, t0, (MAX_HARTID * 4)
	addi t0, t0, (MIN_HARTID * 4)
	li t2, 1
1:	sw t2, 0(t0)
	addi t0, t0, 4
	bne t1, t0, 1b

wait:   /* Wait for initilization to finish. */
	/* Wait on software interrupt */
	csrw mie, MIE_MSIE
	wfi
	csrr t0, mip
	andi t0, t0, MIP_MSIP
	beqz t0, wait

	/* Disable software interrupt. */
	csrw mie, 0
	la t0, _clint
	csrr t1, mhartid
	slli t1, t1, 2
	add t0, t0, t1
	sw x0, (t0)

head_exit:
	csrw mcounteren,0x3
	csrw scounteren,0x3
	csrw mcountinhibit,0x8
	csrw mhpmcounter3,0
	/* Enable timer interrupts */
	li t0, MIE_MTIE
	csrw mie,t0
	/* Start user processes. */
	li a0, 0
	la ra, trap_exit
	tail sched
