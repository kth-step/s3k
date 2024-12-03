// See LICENSE file for copyright and license details.
#include "macro.inc"
#include "offsets.h"
#include "csr.h"

.globl trap_entry
.globl trap_resume
.globl trap_return

.type trap_entry, @function
.type trap_exit, @function

.section .text.trap
.balign 16
trap_entry:
	// Save user a0 to scratch, load PCB pointer
	csrrw	a0,mscratch,a0

	sw	ra,PROC_RA(a0)
	sw	sp,PROC_SP(a0)
	sw	gp,PROC_GP(a0)
	sw	tp,PROC_TP(a0)
	sw	t0,PROC_T0(a0)
	sw	t1,PROC_T1(a0)
	sw	t2,PROC_T2(a0)
	sw	s0,PROC_S0(a0)
	sw	s1,PROC_S1(a0)
	/*sw	a0,PROC_A0(a0)*/
	sw	a1,PROC_A1(a0)
	sw	a2,PROC_A2(a0)
	sw	a3,PROC_A3(a0)
	sw	a4,PROC_A4(a0)
	sw	a5,PROC_A5(a0)
	sw	a6,PROC_A6(a0)
	sw	a7,PROC_A7(a0)
	sw	s2,PROC_S2(a0)
	sw	s3,PROC_S3(a0)
	sw	s4,PROC_S4(a0)
	sw	s5,PROC_S5(a0)
	sw	s6,PROC_S6(a0)
	sw	s7,PROC_S7(a0)
	sw	s8,PROC_S8(a0)
	sw	s9,PROC_S9(a0)
	sw	s10,PROC_S10(a0)
	sw	s11,PROC_S11(a0)
	sw	t3,PROC_T3(a0)
	sw	t4,PROC_T4(a0)
	sw	t5,PROC_T5(a0)
	sw	t6,PROC_T6(a0)
	
	// Save the 
	csrr	t1,mepc
	csrr	t2,mscratch
	sw	t1,PROC_PC(a0)
	sw	t2,PROC_A0(a0)


	// Load the global and stack pointer of the kernel.
	ld_gp
	ld_sp	t0

	// call the trap handler
	csrr	a1,mcause
	csrr	a2,mtval
	call	trap_handler

trap_resume:
	lw	s0,PROC_PMPADDR0(a0)
	lw	s1,PROC_PMPADDR1(a0)
	lw	s2,PROC_PMPADDR2(a0)
	lw	s3,PROC_PMPADDR3(a0)
	lw	s4,PROC_PMPADDR4(a0)
	lw	s5,PROC_PMPADDR5(a0)
	lw	s6,PROC_PMPADDR6(a0)
	lw	s7,PROC_PMPADDR7(a0)
	lw	s8,PROC_PMPCFG0(a0)
	csrw	pmpaddr0,s0
	csrw	pmpaddr1,s1
	csrw	pmpaddr2,s2
	csrw	pmpaddr3,s3
	csrw	pmpaddr4,s4
	csrw	pmpaddr5,s5
	csrw	pmpaddr6,s6
	csrw	pmpaddr7,s7
	csrw	pmpcfg0,s8

trap_return:
	lw	t0,PROC_PC(a0)
	lw	t1,PROC_A0(a0)
	csrw	mepc,t0
	csrw	mscratch,t1

	lw	ra,PROC_RA(a0)
	lw	sp,PROC_SP(a0)
	lw	gp,PROC_GP(a0)
	lw	tp,PROC_TP(a0)
	lw	t0,PROC_T0(a0)
	lw	t1,PROC_T1(a0)
	lw	t2,PROC_T2(a0)
	lw	s0,PROC_S0(a0)
	lw	s1,PROC_S1(a0)
	/*lw	a0,PROC_A0(a0)*/
	lw	a1,PROC_A1(a0)
	lw	a2,PROC_A2(a0)
	lw	a3,PROC_A3(a0)
	lw	a4,PROC_A4(a0)
	lw	a5,PROC_A5(a0)
	lw	a6,PROC_A6(a0)
	lw	a7,PROC_A7(a0)
	lw	s2,PROC_S2(a0)
	lw	s3,PROC_S3(a0)
	lw	s4,PROC_S4(a0)
	lw	s5,PROC_S5(a0)
	lw	s6,PROC_S6(a0)
	lw	s7,PROC_S7(a0)
	lw	s8,PROC_S8(a0)
	lw	s9,PROC_S9(a0)
	lw	s10,PROC_S10(a0)
	lw	s11,PROC_S11(a0)
	lw	t3,PROC_T3(a0)
	lw	t4,PROC_T4(a0)
	lw	t5,PROC_T5(a0)
	lw	t6,PROC_T6(a0)

	// Save PCB pointer.
	csrrw	a0,mscratch,a0

	mret
