# See LICENSE file for copyright and license details.
#include "macro.inc"
#include "offsets.h"
#include "csr.h"

.globl trap_entry
.globl trap_exit
.globl trap_resume

.type trap_entry, @function
.type trap_exit, @function
.type trap_resume, @function

.section .text.trap
.balign 16
trap_entry:
	# Save user tp to scratch, load PCB pointer
	csrrw	tp,mscratch,tp
	beqz	tp,_machine_yield

	sd	ra,PROC_RA(tp)
	sd	sp,PROC_SP(tp)
	sd	gp,PROC_GP(tp)
	#sd	tp,PROC_TP(tp)
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


	# Load the global and stack pointer of the kernel.
	ld_gp
	ld_sp	t0

	csrr	t0,mcause
	bltz	t0,_user_yield

	csrs	mstatus,MSTATUS_MIE

_exception:
1:	auipc	ra,%pcrel_hi(_exception_table)
	slli	t0,t0,2
	add	ra,ra,t0
	jalr	ra,%pcrel_lo(1b)(ra)
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
	ld_gp
	ld_sp	t0
_user_yield:
	call	sched
	csrs	mstatus,MSTATUS_MIE

trap_exit:
	ld	t0,PROC_PC(tp)
	csrw	mepc,t0

	ld	ra,PROC_RA(tp)
	ld	sp,PROC_SP(tp)
	ld	gp,PROC_GP(tp)
	#ld	tp,PROC_TP(tp)
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

	# disable interrupt
	csrw	mstatus,zero

	# Save PCB pointer
	csrw	mscratch,tp

	# Load user tp
	ld	tp,PROC_TP(tp)

	mret

__hang:
	ebreak
	j __hang
