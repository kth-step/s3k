// See LICENSE file for copyright and license details.
#include "macro.inc"
#include "offsets.h"
#include "csr.h"

.globl trap_entry
.globl trap_exit_pmp
.globl trap_exit
.globl trap_sched

.type trap_entry, @function
.type trap_exit, @function
.type trap_resume, @function
.type trap_sched, @function

.section .text.trap
.balign 16
trap_entry:
	// Save user a0 to scratch, load PCB pointer
	csrrw	tp,mscratch,tp
	beqz	tp,_machine_yield

	sd	ra,PROC_RA(tp)
	sd	sp,PROC_SP(tp)
	sd	gp,PROC_GP(tp)
	// sd	tp,PROC_TP(tp)
	sd	t0,PROC_T0(tp)
	sd	t1,PROC_T1(tp)
	sd	t2,PROC_T2(tp)
	sd	s0,PROC_S0(tp)
	sd	s1,PROC_S1(tp)
	sd	a0,PROC_A0(tp)
	sd	a1,PROC_A1(tp)
	sd	a2,PROC_A2(tp)
	sd	a3,PROC_A3(tp)
	sd	a4,PROC_A4(tp)
	sd	a5,PROC_A5(tp)
	sd	a6,PROC_A6(tp)
	sd	a7,PROC_A7(tp)
	sd	s2,PROC_S2(tp)
	sd	s3,PROC_S3(tp)
	sd	s4,PROC_S4(tp)
	sd	s5,PROC_S5(tp)
	sd	s6,PROC_S6(tp)
	sd	s7,PROC_S7(tp)
	sd	s8,PROC_S8(tp)
	sd	s9,PROC_S9(tp)
	sd	s10,PROC_S10(tp)
	sd	s11,PROC_S11(tp)
	sd	t3,PROC_T3(tp)
	sd	t4,PROC_T4(tp)
	sd	t5,PROC_T5(tp)
	sd	t6,PROC_T6(tp)
	csrr	t1,mepc
	sd	t1,PROC_PC(tp)
	csrrw	t2,mscratch,zero
	sd	t2,PROC_TP(tp)


	// Load the global and stack pointer of the kernel.
	ld_gp
	ld_sp	t0

	csrr	t0,mcause
	bltz	t0,trap_sched

#ifndef NPREEMPT
	csrs	mstatus,MSTATUS_MIE
#endif

trap_exception:
	// Load handler address.
1:	auipc	ra,%pcrel_hi(_exception_table)
	slli	t0,t0,2
	add	ra,ra,t0
#ifndef NPREEMPT
	// Disable preemption.
	csrc	mstatus,MSTATUS_MIE	
#endif

	// Call handler.
	jalr	ra,%pcrel_lo(1b)(ra)	

	beq	a0,tp,trap_resume

	// Release old process.
	li	t1,~1
	amoand.d.aqrl	x0,t1,(tp)
	beqz	a0,_sched
	j	trap_exit

_exception_table:
.option push
.option norvc
	j	handle_exception
	j	handle_exception
	j	handle_illegal_instruction
	j	handle_exception
	j	handle_exception
	j	handle_exception
	j	handle_exception
	j	handle_exception
	j	handle_syscall
	j	handle_exception
	j	handle_exception
	j	handle_exception
	j	handle_exception
	j	handle_exception
	j	handle_exception
	j	handle_exception
.option pop

_machine_yield:
	csrrw	tp,mscratch,tp
#ifndef NDEBUG
	csrr	t0,mcause
	li	t1,0x8000000000000007
	bne	t0,t1,__hang
#endif
	csrw	mstatus,0

trap_sched:
	// Release old process.
	li	t1,~1
	amoand.d.aqrl	x0,t1,(tp)

	// Call scheduler.
_sched:
	call	sched

trap_exit:
	mv	tp,a0
#ifndef PREEMPT
	// Enable preemption
	csrs	mstatus,MSTATUS_MIE
#endif
	ld	s0,PROC_PMPADDR0(tp)
	ld	s1,PROC_PMPADDR1(tp)
	ld	s2,PROC_PMPADDR2(tp)
	ld	s3,PROC_PMPADDR3(tp)
	ld	s4,PROC_PMPADDR4(tp)
	ld	s5,PROC_PMPADDR5(tp)
	ld	s6,PROC_PMPADDR6(tp)
	ld	s7,PROC_PMPADDR7(tp)
	ld	s8,PROC_PMPCFG0(tp)
	csrw	pmpaddr0,s0
	csrw	pmpaddr1,s1
	csrw	pmpaddr2,s2
	csrw	pmpaddr3,s3
	csrw	pmpaddr4,s4
	csrw	pmpaddr5,s5
	csrw	pmpaddr6,s6
	csrw	pmpaddr7,s7
	csrw	pmpcfg0,s8

trap_resume:
#ifndef PREEMPT
	csrs	mstatus,MSTATUS_MIE
#endif
	ld	t0,PROC_PC(tp)
	csrw	mepc,t0

	ld	ra,PROC_RA(tp)
	// ld	sp,PROC_SP(tp)
	// ld	gp,PROC_GP(tp)
	// ld	tp,PROC_TP(tp)
	ld	t0,PROC_T0(tp)
	ld	t1,PROC_T1(tp)
	ld	t2,PROC_T2(tp)
	ld	s0,PROC_S0(tp)
	ld	s1,PROC_S1(tp)
	ld	a0,PROC_A0(tp)
	ld	a1,PROC_A1(tp)
	ld	a2,PROC_A2(tp)
	ld	a3,PROC_A3(tp)
	ld	a4,PROC_A4(tp)
	ld	a5,PROC_A5(tp)
	ld	a6,PROC_A6(tp)
	ld	a7,PROC_A7(tp)
	ld	s2,PROC_S2(tp)
	ld	s3,PROC_S3(tp)
	ld	s4,PROC_S4(tp)
	ld	s5,PROC_S5(tp)
	ld	s6,PROC_S6(tp)
	ld	s7,PROC_S7(tp)
	ld	s8,PROC_S8(tp)
	ld	s9,PROC_S9(tp)
	ld	s10,PROC_S10(tp)
	ld	s11,PROC_S11(tp)
	ld	t3,PROC_T3(tp)
	ld	t4,PROC_T4(tp)
	ld	t5,PROC_T5(tp)
	ld	t6,PROC_T6(tp)

	// Disable interrupts.
	csrc	mstatus,MSTATUS_MIE

	// Save PCB pointer.
	csrw	mscratch,tp

	// Load user sp, gp and tp.
	ld	sp,PROC_SP(tp)
	ld	gp,PROC_GP(tp)
	ld	tp,PROC_TP(tp)

	mret

__hang:
	nop
	j __hang
