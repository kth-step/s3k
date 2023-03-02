/* See LICENSE file for copyright and license details. */
//#include "offsets.h"
#include "stack.h"
.globl trap_entry
.globl trap_exit

.section .text.trap
.balign 16
trap_entry:
	// save user a0 to scratch, load PCB
	csrrw	a0,mscratch,a0
	// if scratch was 0, then exception/interrupt in kernel
	beqz	a0,_hang

	// save user registers (except a0 and pc)
	sd	ra,(1*8)(a0)
	sd	sp,(2*8)(a0)
	sd	gp,(3*8)(a0)
	sd	tp,(4*8)(a0)
	sd	t0,(5*8)(a0)
	sd	t1,(6*8)(a0)
	sd	t2,(7*8)(a0)
	sd	s0,(8*8)(a0)
	sd	s1,(9*8)(a0)
	//sd	a0,(10*8)(a0)
	sd	a1,(11*8)(a0)
	sd	a2,(12*8)(a0)
	sd	a3,(13*8)(a0)
	sd	a4,(14*8)(a0)
	sd	a5,(15*8)(a0)
	sd	a6,(16*8)(a0)
	sd	a7,(17*8)(a0)
	sd	s2,(18*8)(a0)
	sd	s3,(19*8)(a0)
	sd	s4,(20*8)(a0)
	sd	s5,(21*8)(a0)
	sd	s6,(22*8)(a0)
	sd	s7,(23*8)(a0)
	sd	s8,(24*8)(a0)
	sd	s9,(25*8)(a0)
	sd	s10,(26*8)(a0)
	sd	s11,(27*8)(a0)
	sd	t3,(28*8)(a0)
	sd	t4,(29*8)(a0)
	sd	t5,(30*8)(a0)
	sd	t6,(31*8)(a0)

	//
	csrrw	t0,mepc,zero	 // load user pc to t0
	csrrw	t1,mscratch,zero // load user a0 to t1
	sd	t0,(0*8)(a0)	 // save pc
	sd	t1,(10*8)(a0)	 // save a0

	// Do not touch t0, t1 as they are used in _syscall
.option push
.option norelax
	la	gp,__global_pointer$
.option pop
	load_sp t2
	mv	tp,a0		// Save a0 (pcb pointer) so we know the process in context   

	csrr	t2,mcause

	bltz	t2,_interrupt	// mcause < 0 ==> interupt
	li	t3,8
	beq	t2,t3,_syscall  // if mcause = 8, then syscall

_exception:
	csrr	a1,mcause
	csrr	a2,mepc
	csrr	a3,mtval
	call	handle_exception
	j	trap_exit

_interrupt:
	call	sched_yield
	j	trap_exit

_syscall: 
	// t0 == user pc
	// t1 == user a0 / system call number

	// t1 < 19 ==> valid syscall
	// t1 >= 19 ==> exception
	li	t2,19
	bge	t1,t2,_exception

	// pc += 4
	addi	t0,t0,4  // incr. pc and save
	sd	t0,(0*8)(tp)

	// compute sycall handler: _syscall_table + (t1 << 2)
1:	auipc	ra,%pcrel_hi(_syscall_table)
	slli	t1,t1,2  
	add	ra,ra,t1
	jalr	ra,%pcrel_lo(1b)(ra)

trap_exit:
	// Load pc and a0
	ld	t0,(0*8)(a0)  // load user pc
	ld	t1,(10*8)(a0) // load user a0
	csrw	mepc,t0	      // set user pc
	csrw	mscratch,t1   // save user a0 to scratch

	// Load other user registers
	ld	ra,(1*8)(a0)
	ld	sp,(2*8)(a0)
	ld	gp,(3*8)(a0)
	ld	tp,(4*8)(a0)
	ld	t0,(5*8)(a0)
	ld	t1,(6*8)(a0)
	ld	t2,(7*8)(a0)
	ld	s0,(8*8)(a0)
	ld	s1,(9*8)(a0)
	//ld	a0,(10*8)(a0)
	ld	a1,(11*8)(a0)
	ld	a2,(12*8)(a0)
	ld	a3,(13*8)(a0)
	ld	a4,(14*8)(a0)
	ld	a5,(15*8)(a0)
	ld	a6,(16*8)(a0)
	ld	a7,(17*8)(a0)
	ld	s2,(18*8)(a0)
	ld	s3,(19*8)(a0)
	ld	s4,(20*8)(a0)
	ld	s5,(21*8)(a0)
	ld	s6,(22*8)(a0)
	ld	s7,(23*8)(a0)
	ld	s8,(24*8)(a0)
	ld	s9,(25*8)(a0)
	ld	s10,(26*8)(a0)
	ld	s11,(27*8)(a0)
	ld	t3,(28*8)(a0)
	ld	t4,(29*8)(a0)
	ld	t5,(30*8)(a0)
	ld	t6,(31*8)(a0)
	// Save PCB pointer, load user a0
	csrrw	a0,mscratch,a0
	mret

_hang:
	wfi
	j	_hang

_syscall_table:
.option push
.option norvc
	j	syscall_getinfo
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
