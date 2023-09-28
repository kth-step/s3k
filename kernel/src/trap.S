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
	/* Save user tp to scratch, load PCB pointer */
	csrrw a0, mscratch, a0
	beqz a0, _myield
1:
	sd ra, TF_RA(a0)
	sd sp, TF_SP(a0)
	sd gp, TF_GP(a0)
	sd tp, TF_TP(a0)
	sd t0, TF_T0(a0)
	sd t1, TF_T1(a0)
	sd t2, TF_T2(a0)
	sd s0, TF_S0(a0)
	sd s1, TF_S1(a0)
	sd s2, TF_S2(a0)
	sd s3, TF_S3(a0)
	sd s4, TF_S4(a0)
	sd s5, TF_S5(a0)
	sd s6, TF_S6(a0)
	sd s7, TF_S7(a0)
	sd s8, TF_S8(a0)
	sd s9, TF_S9(a0)
	sd s10, TF_S10(a0)
	sd s11, TF_S11(a0)
	sd a1, TF_A1(a0)
	sd a2, TF_A2(a0)
	sd a3, TF_A3(a0)
	sd a4, TF_A4(a0)
	sd a5, TF_A5(a0)
	sd a6, TF_A6(a0)
	sd a7, TF_A7(a0)
	sd t3, TF_T3(a0)
	sd t4, TF_T4(a0)
	sd t5, TF_T5(a0)
	sd t6, TF_T6(a0)
	csrr t1, mepc
	sd t1, TF_PC(a0)
	csrrw t2, mscratch, 0
	sd t2, TF_A0(a0)

	/* Load the global and stack pointer of the kernel. */
	load_gp
	load_sp t0

	/* Check if system call. */
	csrr t0, mcause
	bltz t0, _yield

	la ra,trap_exit

	li t1, MCAUSE_USER_ECALL
	beq t0, t1, _syscall

_exception:
	/* Otherwise, it is exception. */
	csrr a1, mcause
	csrr a2, mepc
	csrr a3, mtval
	tail handle_exception

_syscall:
	tail handle_syscall

_myield:
	csrrw a0, mscratch, a0
	load_gp
	load_sp t1

_yield:
	/* Call scheduler */
	call sched

trap_exit:
	csrs mstatus,MSTATUS_MIE

	/* Load call-used registers, ra, sp, gp, tp, pc */
	ld t0, TF_PC(a0)
	csrw mepc,t0

	ld ra, TF_RA(a0)
	ld sp, TF_SP(a0)
	ld gp, TF_GP(a0)
	ld tp, TF_TP(a0)
	ld t0, TF_T0(a0)
	ld t1, TF_T1(a0)
	ld t2, TF_T2(a0)
	ld s0, TF_S0(a0)
	ld s1, TF_S1(a0)
	ld s2, TF_S2(a0)
	ld s3, TF_S3(a0)
	ld s4, TF_S4(a0)
	ld s5, TF_S5(a0)
	ld s6, TF_S6(a0)
	ld s7, TF_S7(a0)
	ld s8, TF_S8(a0)
	ld s9, TF_S9(a0)
	ld s10, TF_S10(a0)
	ld s11, TF_S11(a0)
	ld a1, TF_A1(a0)
	ld a2, TF_A2(a0)
	ld a3, TF_A3(a0)
	ld a4, TF_A4(a0)
	ld a5, TF_A5(a0)
	ld a6, TF_A6(a0)
	ld a7, TF_A7(a0)
	ld t3, TF_T3(a0)
	ld t4, TF_T4(a0)
	ld t5, TF_T5(a0)
	ld t6, TF_T6(a0)

	csrw mstatus,MSTATUS_MIE

	/* Save PCB pointer */
	csrw mscratch, a0

	/* Load user tp */
	ld a0, TF_A0(a0)

	mret

__hang:
	j __hang
