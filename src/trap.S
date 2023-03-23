// See LICENSE file for copyright and license details.
#include "macro.S"
.globl trap_entry
.globl trap_exit

.section .text.trap
.balign  16

trap_entry:
	// save user tp to scratch, load PCB
	csrrw tp, mscratch, tp

	// if scratch was 0, then exception/interrupt in kernel
	beqz tp, _machine_interrupt

	// save user registers (except tp and pc)
	sd ra, (1*8)(tp)
	sd sp, (2*8)(tp)
	sd gp, (3*8)(tp)
	sd t0, (5*8)(tp)
	sd t1, (6*8)(tp)
	sd t2, (7*8)(tp)
	sd s0, (8*8)(tp)
	sd s1, (9*8)(tp)
	sd a0, (10*8)(tp)
	sd a1, (11*8)(tp)
	sd a2, (12*8)(tp)
	sd a3, (13*8)(tp)
	sd a4, (14*8)(tp)
	sd a5, (15*8)(tp)
	sd a6, (16*8)(tp)
	sd a7, (17*8)(tp)
	sd s2, (18*8)(tp)
	sd s3, (19*8)(tp)
	sd s4, (20*8)(tp)
	sd s5, (21*8)(tp)
	sd s6, (22*8)(tp)
	sd s7, (23*8)(tp)
	sd s8, (24*8)(tp)
	sd s9, (25*8)(tp)
	sd s10, (26*8)(tp)
	sd s11, (27*8)(tp)
	sd t3, (28*8)(tp)
	sd t4, (29*8)(tp)
	sd t5, (30*8)(tp)
	sd t6, (31*8)(tp)

	csrr  t0, mepc     // load user pc to t0
	csrrw t1, mscratch, zero // load user tp to t1
	sd    t0, (0*8)(tp)      // save pc
	sd    t1, (4*8)(tp)      // save tp

	// Do not touch t0, t1 as they are used in _syscall
	.option push
	.option norelax
	la      gp, __global_pointer$
	.option pop
	load_sp t1

	csrr t1, mcause

	bltz t1, _interrupt   // mcause < 0 ==> interupt
	li   t2, 8
	beq  t1, t2, _syscall // if mcause = 8, then syscall

_exception:
	mv   a0, tp           // Save a0 (pcb pointer) so we know the process in context
	csrr a1, mcause
	csrr a2, mepc
	csrr a3, mtval
	call handle_exception
	j    trap_exit

_machine_interrupt:
	csrrw	tp,mscratch,zero
	.option push
	.option norelax
	la      gp, __global_pointer$
	.option pop
	load_sp t1
_interrupt:
	mv   a0, tp         // Save a0 (pcb pointer) so we know the process in context
	call schedule_yield
	j    trap_exit

_syscall:
	// t0 == user pc
	// a0 == user a0 / system call number

	// a0 < 19 ==> valid syscall
	// a0 >= 19 ==> exception
	li  t1, 19
	bge a0, t1, _exception

	// pc += 4
	addi t0, t0, 4
	sd   t0, (0*8)(tp)

	// compute sycall handler: _syscall_table + (a0 << 2)
1:
	auipc ra, %pcrel_hi(_syscall_table)
	slli  a0, a0, 2
	add   ra, ra, a0
	mv    a0, tp                        // Save a0 (pcb pointer) so we know the process in context
	jalr  ra, %pcrel_lo(1b)(ra)

trap_exit:
	csrw mstatus,0x8
	// Load pc and a0
	ld t0, (0*8)(tp)
	csrw mepc,t0      // set user pc

	// Load other user registers
	ld ra, (1*8)(tp)
	ld sp, (2*8)(tp)
	ld gp, (3*8)(tp)
	ld t0, (5*8)(tp)
	ld t1, (6*8)(tp)
	ld t2, (7*8)(tp)
	ld s0, (8*8)(tp)
	ld s1, (9*8)(tp)
	ld a0, (10*8)(tp)
	ld a1, (11*8)(tp)
	ld a2, (12*8)(tp)
	ld a3, (13*8)(tp)
	ld a4, (14*8)(tp)
	ld a5, (15*8)(tp)
	ld a6, (16*8)(tp)
	ld a7, (17*8)(tp)
	ld s2, (18*8)(tp)
	ld s3, (19*8)(tp)
	ld s4, (20*8)(tp)
	ld s5, (21*8)(tp)
	ld s6, (22*8)(tp)
	ld s7, (23*8)(tp)
	ld s8, (24*8)(tp)
	ld s9, (25*8)(tp)
	ld s10, (26*8)(tp)
	ld s11, (27*8)(tp)
	ld t3, (28*8)(tp)
	ld t4, (29*8)(tp)
	ld t5, (30*8)(tp)
	ld t6, (31*8)(tp)

	csrw mstatus,0
	csrw mscratch, tp
	ld   tp,(4*8)(tp) // load user tp
	mret

_hang:
	wfi
	j _hang

_syscall_table:
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

syscall_table_end:
