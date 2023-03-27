// See LICENSE file for copyright and license details.
#include "macro.S"
.globl trap_entry
.globl trap_exit

#define PC_OFFSET 0
#define RA_OFFSET 8
#define SP_OFFSET 16
#define GP_OFFSET 24
#define TP_OFFSET 32
#define T0_OFFSET 40
#define T1_OFFSET 48
#define T2_OFFSET 56
#define S0_OFFSET 64
#define S1_OFFSET 72
#define A0_OFFSET 80
#define A1_OFFSET 88
#define A2_OFFSET 96
#define A3_OFFSET 104
#define A4_OFFSET 112
#define A5_OFFSET 120
#define A6_OFFSET 128
#define A7_OFFSET 136
#define S2_OFFSET 144
#define S3_OFFSET 152
#define S4_OFFSET 160
#define S5_OFFSET 168
#define S6_OFFSET 176
#define S7_OFFSET 184
#define S8_OFFSET 192
#define S9_OFFSET 200
#define S10_OFFSET 208
#define S11_OFFSET 216
#define T3_OFFSET 224
#define T4_OFFSET 232
#define T5_OFFSET 240
#define T6_OFFSET 248

#define USER_ECALL 8

#define SYSCALL_COUNT 19

#define MSTATUS_MIE 8

.section .text.trap
.balign  16

trap_entry:
	// save user tp to scratch, load PCB
	csrrw tp, mscratch, tp

	// if scratch was 0, then exception/interrupt in kernel
	beqz tp, .machine_interrupt

	// save user registers (except tp and pc)
	sd ra, (RA_OFFSET)(tp)
	sd sp, (SP_OFFSET)(tp)
	sd gp, (GP_OFFSET)(tp)
	sd t0, (T0_OFFSET)(tp)
	sd t1, (T1_OFFSET)(tp)
	sd t2, (T2_OFFSET)(tp)
	sd s0, (S0_OFFSET)(tp)
	sd s1, (S0_OFFSET)(tp)
	sd a0, (A0_OFFSET)(tp)
	sd a1, (A1_OFFSET)(tp)
	sd a2, (A2_OFFSET)(tp)
	sd a3, (A3_OFFSET)(tp)
	sd a4, (A4_OFFSET)(tp)
	sd a5, (A5_OFFSET)(tp)
	sd a6, (A6_OFFSET)(tp)
	sd a7, (A7_OFFSET)(tp)
	sd s2, (S2_OFFSET)(tp)
	sd s3, (S3_OFFSET)(tp)
	sd s4, (S4_OFFSET)(tp)
	sd s5, (S5_OFFSET)(tp)
	sd s6, (S6_OFFSET)(tp)
	sd s7, (S7_OFFSET)(tp)
	sd s8, (S8_OFFSET)(tp)
	sd s9, (S9_OFFSET)(tp)
	sd s10,(S10_OFFSET)(tp)
	sd s11,(S11_OFFSET)(tp)
	sd t3, (T3_OFFSET)(tp)
	sd t4, (T4_OFFSET)(tp)
	sd t5, (T5_OFFSET)(tp)
	sd t6, (T6_OFFSET)(tp)

	// load user pc and tp
	csrr  t0, mepc     
	csrrw t1, mscratch, zero 
	// save user pc and tp
	sd    t0, (PC_OFFSET)(tp)
	sd    t1, (TP_OFFSET)(tp)

	// Do not touch t0, t1 as they are used in syscall
	.option push
	.option norelax
	la      gp, __global_pointer$
	.option pop
	load_sp t1

	csrr t1, mcause

	bltz t1, .interrupt   // mcause < 0 ==> interupt
	li   t2, USER_ECALL
	beq  t1, t2, .syscall // if mcause = 8, then syscall

	// fallthrough

	// This subroutine handles user exceptions.
	// We call handle_exception(proc, mcause, mepc, mtval).
	// Aftwerwards we go to trap exit.
	.exception:
		// Save a0 (pcb pointer) so we know the process in context
		mv   a0, tp
		csrr a1, mcause
		csrr a2, mepc
		csrr a3, mtval
		call handle_exception
		j    trap_exit

	// This subroutine handles machine interrupt.
	// We only have timer interrupts in machine mode.
	// We fix the thread pointer, global pointer and the stack pointer
	// before continuing to the interrupt subroutine.
	.machine_interrupt:
		csrrw	tp,mscratch,zero
		.option push
		.option norelax
		la      gp, __global_pointer$
		.option pop
		load_sp t1
		// fallthrough

	// This subroutine handles interrupts.
	// We only have timer interrupts so we invoke the scheduler
	// schedule_yield(proc).
	.interrupt:
		mv   a0, tp
		call schedule_yield
		j    trap_exit

	// This subroutine handles syscalls. We check that the syscall number is
	// valid, if it is invalid we have an exception and jump to the 
	// exception handler. If the syscall is valid, we increment the PC
	// with 4 (ecall instruction is 4 bytes wide), calculate the target
	// address of the trampoline, and jump to the trampoline entry. Syscall
	// handlers are in a trampoline with 4 byte wide jump instructions.
	//
	// We assume that the user process's PC is in register t0, and that the
	// syscall number is in register a0. After the syscall has been handled,
	// we go to trap_exit.
	.syscall:
		// Check syscall number.
		li  t1, SYSCALL_COUNT
		bge a0, t1, .exception
	
		// increment PC
		addi t0, t0, 4
		sd   t0, (PC_OFFSET)(tp)

		// compute trampoline address.
	1:	auipc ra, %pcrel_hi(.syscall_table)
		slli  a0, a0, 2
		add   ra, ra, a0
		// Move tp to a0, so we call handler(proc, a1, a2, ..., a7).
		mv    a0, tp
		jalr  ra, %pcrel_lo(1b)(ra)

	// fallthrough

// This function restores the user context.
// The function is preemptable while the tp points to the PCB.
trap_exit:
	// Enable preemption by setting the machine interrupt enable bit
	// in mstatus
	csrw mstatus,MSTATUS_MIE

	// Load user pc to mepc.
	ld t0, (PC_OFFSET)(tp)
	csrw mepc,t0

	// Load other user registers
	ld ra, (RA_OFFSET)(tp)
	ld sp, (SP_OFFSET)(tp)
	ld gp, (GP_OFFSET)(tp)
	ld t0, (T0_OFFSET)(tp)
	ld t1, (T1_OFFSET)(tp)
	ld t2, (T2_OFFSET)(tp)
	ld s0, (S0_OFFSET)(tp)
	ld s1, (S1_OFFSET)(tp)
	ld a0, (A0_OFFSET)(tp)
	ld a1, (A1_OFFSET)(tp)
	ld a2, (A2_OFFSET)(tp)
	ld a3, (A3_OFFSET)(tp)
	ld a4, (A4_OFFSET)(tp)
	ld a5, (A5_OFFSET)(tp)
	ld a6, (A6_OFFSET)(tp)
	ld a7, (A7_OFFSET)(tp)
	ld s2, (S2_OFFSET)(tp)
	ld s3, (S3_OFFSET)(tp)
	ld s4, (S4_OFFSET)(tp)
	ld s5, (S5_OFFSET)(tp)
	ld s6, (S6_OFFSET)(tp)
	ld s7, (S7_OFFSET)(tp)
	ld s8, (S8_OFFSET)(tp)
	ld s9, (S9_OFFSET)(tp)
	ld s10,(S10_OFFSET)(tp)
	ld s11,(S11_OFFSET)(tp)
	ld t3, (T3_OFFSET)(tp)
	ld t4, (T4_OFFSET)(tp)
	ld t5, (T5_OFFSET)(tp)
	ld t6, (T6_OFFSET)(tp)

	// Disable preemption
	csrw mstatus,0
	// Save the PCB pointer to mscratch.
	csrw mscratch, tp
	// Load the user thread pointer.
	ld   tp,(TP_OFFSET)(tp) // load user tp
	// Return to the user.
	mret

// This is used for debugging.
.hang:
	wfi
	j .hang

.syscall_table:
	.option push
	.option norvc
	j       syscall_getinfo
	j       syscall_getreg
	j       syscall_setreg
	j       syscall_yield
	j       syscall_getcap
	j       syscall_movcap
	j       syscall_delcap
	j       syscall_revcap
	j       syscall_drvcap
	j       syscall_msuspend
	j       syscall_mresume
	j       syscall_mgetreg
	j       syscall_msetreg
	j       syscall_mgetcap
	j       syscall_mtakecap
	j       syscall_mgivecap
	j       syscall_recv
	j       syscall_send
	j       syscall_sendrecv
	.option pop
