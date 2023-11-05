# See LICENSE file for copyright and license details.
#include "macro.inc"
#include "offsets.h"
#include "csr.h"

.globl trap_entry
.globl trap_exit_pmp
.globl trap_exit

.type trap_entry, @function
.type trap_exit_pmp, @function
.type trap_exit, @function

.section .text.trap
.balign 16
trap_entry:
	# Save user a0 to scratch, load PCB pointer
	csrrw	a0,mscratch,a0
	beqz	a0,_machine_yield

	sd	ra,PROC_RA(a0)
	sd	sp,PROC_SP(a0)
	sd	gp,PROC_GP(a0)
	sd	tp,PROC_TP(a0)
	sd	t0,PROC_T0(a0)
	sd	t1,PROC_T1(a0)
	sd	t2,PROC_T2(a0)
	sd	s0,PROC_S0(a0)
	sd	s1,PROC_S1(a0)
	#sd	a0,PROC_A0(a0)
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
	csrr	t1,mepc
	sd	t1,PROC_PC(a0)
	csrrw	t2,mscratch,zero
	sd	t2,PROC_A0(a0)


	# Load the global and stack pointer of the kernel.
	ld_gp
	ld_sp	t0

	csrr	t0,mcause
	bltz	t0,_user_yield

	csrs	mstatus,MSTATUS_MIE

_exception:
	# Save current process
	mv	tp,a0

	# Load handler address
1:	auipc	ra,%pcrel_hi(_exception_table)
	slli	t0,t0,2
	add	ra,ra,t0
	csrc	mstatus,MSTATUS_MIE	# disable preemption
	jalr	ra,%pcrel_lo(1b)(ra)	# Call handler

	# If a0 == tp, exit
	beq	a0,tp,trap_exit

	li	t1,~1
	amoand.d.rl	x0,t1,(tp)


	# If a0 != NULL, exit
	bnez	a0,trap_exit_pmp

	call sched

	j	trap_exit_pmp

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
	csrrw	a0,mscratch,a0
_user_yield:

	csrr	t0,mcause
	li	t1,0x8000000000000007
	bne	t0,t1,__hang

	# release old process
	li	t1,~1
	amoand.d.rl	x0,t1,(a0)

	# call scheduler
	call	sched

trap_exit_pmp:
	ld	s0,PROC_PMPADDR0(a0)
	ld	s1,PROC_PMPADDR1(a0)
	ld	s2,PROC_PMPADDR2(a0)
	ld	s3,PROC_PMPADDR3(a0)
	ld	s4,PROC_PMPADDR4(a0)
	ld	s5,PROC_PMPADDR5(a0)
	ld	s6,PROC_PMPADDR6(a0)
	ld	s7,PROC_PMPADDR7(a0)
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

trap_exit:
	ld	t0,PROC_PC(a0)
	csrw	mepc,t0

	ld	ra,PROC_RA(a0)
	#ld	sp,PROC_A0(a0)
	#ld	gp,PROC_A0(a0)
	ld	tp,PROC_TP(a0)
	ld	t0,PROC_T0(a0)
	ld	t1,PROC_T1(a0)
	ld	t2,PROC_T2(a0)
	ld	s0,PROC_S0(a0)
	ld	s1,PROC_S1(a0)
	#ld	a0,PROC_A0(a0)
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

	# disable interrupt
	csrw	mstatus,zero

	# Save PCB pointer
	csrw	mscratch,a0

	# Load user sp, gp and a0
	ld	sp,PROC_SP(a0)
	ld	gp,PROC_GP(a0)
	ld	a0,PROC_A0(a0)

	mret

__hang:
	nop
	j __hang
