# See LICENSE file for copyright and license details.
#include "macro.inc"
#include "offsets.h"
#include "csr.h"

.extern init_kernel
.extern sched
.extern trap_entry
.extern trap_exit
.extern trap_exit_pmp

.section .text.init,"ax",@progbits

.globl _start
.type _start, @function
_start:
head_entry:
	/* Load global pointer */
	ld_gp
	ld_sp	t1

	/* If hartid != MIN_HARTID, then jump to wait. */
	csrr	t0,mhartid
	li	t1,S3K_MIN_HART
	li	t2,S3K_MAX_HART
	bltu	t0,t1,__hang
	bgtu	t0,t2,__hang

	/* Set some CSRs to 0 */
	csrw	mstatus,0
	csrw	medeleg,0
	csrw	mideleg,0
	csrw	mscratch,0
	csrw	mie,0
	csrw	satp,0

	/* Set trap entry. */
	la	t0,__hang
	csrw	mtvec,t0

	csrr	t0,mhartid
	li	t1,S3K_MIN_HART
	bne	t0,t1,wait

zero_bss: /* write zeros to the bss section */
	la	t0,_bss
	la 	t1,_end
	j	2f
1:	sb	zero,(t0)
	addi	t0,t0,1
2:	bne	t0,t1,1b

	call	kernel_init

	la	t0,kernel_ready
	li	t1,1
	sw	t1,(t0)

wait:   # Wait for initilization to finish.
	la	t0,kernel_ready
	lw	t0,(t0)
	beqz	t0,wait

head_exit:
	# Enable timer interrupts
	li	t0,MIE_MTIE
	csrw	mie,t0

#ifdef INSTRUMENT
	csrw	mcounteren,0xF
	csrw	scounteren,0xF
#endif

	# Start user processes.
	la	t0,trap_entry
	csrw	mtvec,t0
	csrw	mscratch,0
	call	sched
	j	trap_exit_pmp

__hang:
	j	__hang

.section .data
kernel_ready:
	.word 0
