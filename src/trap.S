/* See LICENSE file for copyright and license details. */
#include "offsets.h"
#include "stack.h"
.globl trap_entry
.globl trap_exit

.section .text.trap
.balign 16
trap_entry:
	csrrw	tp,mscratch,tp
	beqz	tp,_hang
	sd	ra,(1*8)(tp)
	sd	sp,(2*8)(tp)
	sd	gp,(3*8)(tp)
	//sd	tp,(4*8)(tp)
	sd	t0,(5*8)(tp)
	sd	t1,(6*8)(tp)
	sd	t2,(7*8)(tp)
	sd	s0,(8*8)(tp)
	sd	s1,(9*8)(tp)
	sd	a0,(10*8)(tp)
	sd	a1,(11*8)(tp)
	sd	a2,(12*8)(tp)
	sd	a3,(13*8)(tp)
	sd	a4,(14*8)(tp)
	sd	a5,(15*8)(tp)
	sd	a6,(16*8)(tp)
	sd	a7,(17*8)(tp)
	sd	s2,(18*8)(tp)
	sd	s3,(19*8)(tp)
	sd	s4,(20*8)(tp)
	sd	s5,(21*8)(tp)
	sd	s6,(22*8)(tp)
	sd	s7,(23*8)(tp)
	sd	s8,(24*8)(tp)
	sd	s9,(25*8)(tp)
	sd	s10,(26*8)(tp)
	sd	s11,(27*8)(tp)
	sd	t3,(28*8)(tp)
	sd	t4,(29*8)(tp)
	sd	t5,(30*8)(tp)
	sd	t6,(31*8)(tp)

	csrrw	t2,mepc,zero
	csrrw	t1,mscratch,zero
	sd	t2,(0*8)(tp)
	sd	t1,(4*8)(tp)

.option push
.option norelax
	la	gp,__global_pointer$
.option pop
	load_sp t0

	csrr	t0,mcause

	bltz	t0,_interrupt	// mcause < 0 ==> interupt
	li	t1,8
	beq	t0,t1,_syscall

_exception:
	csrr	a0,mcause
	csrr	a1,mepc
	csrr	a2,mtval
	call	handle_exception
	j	trap_exit

_interrupt:
	call	sched_yield
	j	trap_exit

_syscall:
	li	t0,19
	bge	a7,t0,_exception
	addi	t2,t2,4
	sd	t2,(0*8)(tp)
1:	auipc	ra,%pcrel_hi(_syscall_table)
	slli	t1,a7,2
	add	ra,ra,t1
	jalr	ra,%pcrel_lo(1b)(ra)
	sd	a0,(10*8)(tp)

trap_exit:
	csrw	mstatus,0
	ld	t0,(0*8)(tp) // pc
	ld	t1,(4*8)(tp) // tp
	csrw	mepc,t0
	csrw	mscratch,t1
	ld	ra,(1*8)(tp)
	ld	sp,(2*8)(tp)
	ld	gp,(3*8)(tp)
	//ld	tp,(4*8)(tp)
	ld	t0,(5*8)(tp)
	ld	t1,(6*8)(tp)
	ld	t2,(7*8)(tp)
	ld	s0,(8*8)(tp)
	ld	s1,(9*8)(tp)
	ld	a0,(10*8)(tp)
	ld	a1,(11*8)(tp)
	ld	a2,(12*8)(tp)
	ld	a3,(13*8)(tp)
	ld	a4,(14*8)(tp)
	ld	a5,(15*8)(tp)
	ld	a6,(16*8)(tp)
	ld	a7,(17*8)(tp)
	ld	s2,(18*8)(tp)
	ld	s3,(19*8)(tp)
	ld	s4,(20*8)(tp)
	ld	s5,(21*8)(tp)
	ld	s6,(22*8)(tp)
	ld	s7,(23*8)(tp)
	ld	s8,(24*8)(tp)
	ld	s9,(25*8)(tp)
	ld	s10,(26*8)(tp)
	ld	s11,(27*8)(tp)
	ld	t3,(28*8)(tp)
	ld	t4,(29*8)(tp)
	ld	t5,(30*8)(tp)
	ld	t6,(31*8)(tp)
	csrrw	tp,mscratch,tp
	mret

_hang:
	wfi
	j	_hang

_syscall_table:
.option push
.option norvc
	j	syscall_getpid
	j	syscall_getreg
	j	syscall_setreg
	j	syscall_yield
	j	syscall_getcap
	j	syscall_movcap
	j	syscall_delcap
	j	syscall_revcap
	j	syscall_drvcap
	j	syscall_msuspend
	j	syscall_mresume
	j	syscall_mgetreg
	j	syscall_msetreg
	j	syscall_mgetcap
	j	syscall_mtakecap
	j	syscall_mgivecap
	j	syscall_recv
	j	syscall_send
	j	syscall_sendrecv
.option pop
syscall_table_end:
