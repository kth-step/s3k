// See LICENSE file for copyright and license details.
#include "macro.inc"
#include "offsets.h"
#include "csr.h"

.globl trap_entry
.globl trap_exit

.type trap_entry, @function
.type trap_exit, @function

.section .text.trap
.balign 16
trap_entry:
	// Save user a0 to scratch, load PCB pointer
	csrrw	a0,mscratch,a0

	sd	ra,PROC_RA(a0)
	sd	sp,PROC_SP(a0)
	sd	gp,PROC_GP(a0)
	sd	tp,PROC_TP(a0)
	sd	t0,PROC_T0(a0)
	sd	t1,PROC_T1(a0)
	sd	t2,PROC_T2(a0)
	sd	s0,PROC_S0(a0)
	sd	s1,PROC_S1(a0)
	/*sd	a0,PROC_A0(a0)*/
	sd	a1,PROC_A1(a0)
	sd	a2,PROC_A2(a0)
	sd	a3,PROC_A3(a0)
	sd	a4,PROC_A4(a0)
	sd	a5,PROC_A5(a0)
	sd	a6,PROC_A6(a0)
	sd	a7,PROC_A7(a0)
	sd	s2,PROC_S2(a0)
	sd	s3,PROC_S3(a0)
	sd	s4,PROC_S4(a0)
	sd	s5,PROC_S5(a0)
	sd	s6,PROC_S6(a0)
	sd	s7,PROC_S7(a0)
	sd	s8,PROC_S8(a0)
	sd	s9,PROC_S9(a0)
	sd	s10,PROC_S10(a0)
	sd	s11,PROC_S11(a0)
	sd	t3,PROC_T3(a0)
	sd	t4,PROC_T4(a0)
	sd	t5,PROC_T5(a0)
	sd	t6,PROC_T6(a0)
	
	// Save the 
	csrr	t1,mepc
	csrr	t2,mscratch
	sd	t1,PROC_PC(a0)
	sd	t2,PROC_A0(a0)


	// Load the global and stack pointer of the kernel.
	ld_gp
	ld_sp	t0

	// call the trap handler
	csrr	a1,mcause
	csrr	a2,mtval
	call	trap_handler

trap_exit:
	ld	s0,PROC_PMPADDR0(a0)
	ld	s1,PROC_PMPADDR1(a0)
	ld	s2,PROC_PMPADDR2(a0)
	ld	s3,PROC_PMPADDR3(a0)
	ld	s4,PROC_PMPADDR4(a0)
	ld	s5,PROC_PMPADDR5(a0)
	ld	s6,PROC_PMPADDR6(a0)
	ld	s7,PROC_PMPADDR7(a0)
	fence
	ld	s8,PROC_PMPCFG0(a0)
	csrw	pmpaddr0,s0
	csrw	pmpaddr1,s1
	csrw	pmpaddr2,s2
	csrw	pmpaddr3,s3
	csrw	pmpaddr4,s4
	csrw	pmpaddr5,s5
	csrw	pmpaddr6,s6
	csrw	pmpaddr7,s7
	csrw	pmpcfg0,s8

	ld	t0,PROC_PC(a0)
	ld	t1,PROC_A0(a0)
	csrw	mepc,t0
	csrw	mscratch,t1

	ld	ra,PROC_RA(a0)
	ld	sp,PROC_SP(a0)
	ld	gp,PROC_GP(a0)
	ld	tp,PROC_TP(a0)
	ld	t0,PROC_T0(a0)
	ld	t1,PROC_T1(a0)
	ld	t2,PROC_T2(a0)
	ld	s0,PROC_S0(a0)
	ld	s1,PROC_S1(a0)
	/*ld	a0,PROC_A0(a0)*/
	ld	a1,PROC_A1(a0)
	ld	a2,PROC_A2(a0)
	ld	a3,PROC_A3(a0)
	ld	a4,PROC_A4(a0)
	ld	a5,PROC_A5(a0)
	ld	a6,PROC_A6(a0)
	ld	a7,PROC_A7(a0)
	ld	s2,PROC_S2(a0)
	ld	s3,PROC_S3(a0)
	ld	s4,PROC_S4(a0)
	ld	s5,PROC_S5(a0)
	ld	s6,PROC_S6(a0)
	ld	s7,PROC_S7(a0)
	ld	s8,PROC_S8(a0)
	ld	s9,PROC_S9(a0)
	ld	s10,PROC_S10(a0)
	ld	s11,PROC_S11(a0)
	ld	t3,PROC_T3(a0)
	ld	t4,PROC_T4(a0)
	ld	t5,PROC_T5(a0)
	ld	t6,PROC_T6(a0)

	// Save PCB pointer.
	csrrw	a0,mscratch,a0

	mret
