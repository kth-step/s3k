/* See LICENSE file for copyright and license details. */
#include "offsets.h"
#include "stack.h"
.globl trap_entry
.globl trap_exit


.extern handle_exception
.extern handle_interrupt
.extern syscall_get_reg
.extern syscall_set_reg
.extern syscall_yield
.extern syscall_read_cap
.extern syscall_move_cap
.extern syscall_delete_cap
.extern syscall_revoke_cap
.extern syscall_derive_cap
.extern syscall_invoke_cap


.section .text.trap
.balign 8
trap_entry:
	csrrw	a0,mscratch,a0
	beqz	a0,_minterrupt

	sd	ra,(REGS_RA)(a0)
	sd	sp,(REGS_SP)(a0)
	sd	gp,(REGS_GP)(a0)
	sd	tp,(REGS_TP)(a0)
	sd	t0,(REGS_T0)(a0)
	sd	t1,(REGS_T1)(a0)
	sd	t2,(REGS_T2)(a0)
	sd	s0,(REGS_S0)(a0)
	sd	s1,(REGS_S1)(a0)
	sd	a1,(REGS_A1)(a0)
	sd	a2,(REGS_A2)(a0)
	sd	a3,(REGS_A3)(a0)
	sd	a4,(REGS_A4)(a0)
	sd	a5,(REGS_A5)(a0)
	sd	a6,(REGS_A6)(a0)
	sd	a7,(REGS_A7)(a0)
	sd	s2,(REGS_S2)(a0)
	sd	s3,(REGS_S3)(a0)
	sd	s4,(REGS_S4)(a0)
	sd	s5,(REGS_S5)(a0)
	sd	s6,(REGS_S6)(a0)
	sd	s7,(REGS_S7)(a0)
	sd	s8,(REGS_S8)(a0)
	sd	s9,(REGS_S9)(a0)
	sd	s10,(REGS_S10)(a0)
	sd	s11,(REGS_S11)(a0)
	sd	t3,(REGS_T3)(a0)
	sd	t4,(REGS_T4)(a0)
	sd	t5,(REGS_T5)(a0)
	sd	t6,(REGS_T6)(a0)

	/* save user a0 */
	csrrw	s1,mscratch,zero
	sd	s1,(REGS_A0)(a0)

.option push
.option norelax
	la	gp,__global_pointer$
.option pop
	load_sp t0
	
	/* check if interrupt */
	csrr	t0,mcause
	bltz	t0,_interrupt
	/* check if exception */
	li	t1,8
	bne	t0,t1,_exception
	/* exception if syscall num >= 10 */
	li	t1,10
	bgeu	s0,t1,_exception

_syscall:
	/* save user pc + 4 */
	csrr	t0,mepc
	addi	t0,t0,4
	sd	t0,(REGS_PC)(a0)
1:	auipc	s0,%pcrel_hi(_syscall_table)
	/* compute syscall table address */
	/* s1 -> user a0 */
	slli	s1,s1,2
	add	s0,s0,s1
2:	jalr	ra,%pcrel_lo(1b)(s0)
	j	trap_exit

_exception:
	csrr	a1,mcause
	csrr	a2,mepc
	csrr	a3,mtval
	call	handle_exception
	j	trap_exit

_minterrupt:
	csrrw	a0,mscratch,zero
.option push
.option norelax
	la	gp,__global_pointer$
.option pop
	load_sp t0

_interrupt:
	// Context switch
	/* save user pc */
	csrr	t0,mepc
	sd	t0,(REGS_PC)(a0)
	call	schedule_yield

trap_exit:
	/* TODO: enable preemption */

	ld	t0,(REGS_PC)(a0)
	csrw	mepc,t0

	ld	ra,(REGS_RA)(a0)
	ld	sp,(REGS_SP)(a0)
	ld	gp,(REGS_GP)(a0)
	ld	tp,(REGS_TP)(a0)
	ld	t0,(REGS_T0)(a0)
	ld	t1,(REGS_T1)(a0)
	ld	t2,(REGS_T2)(a0)
	ld	s0,(REGS_S0)(a0)
	ld	s1,(REGS_S1)(a0)
	ld	a1,(REGS_A1)(a0)
	ld	a2,(REGS_A2)(a0)
	ld	a3,(REGS_A3)(a0)
	ld	a4,(REGS_A4)(a0)
	ld	a5,(REGS_A5)(a0)
	ld	a6,(REGS_A6)(a0)
	ld	a7,(REGS_A7)(a0)
	ld	s2,(REGS_S2)(a0)
	ld	s3,(REGS_S3)(a0)
	ld	s4,(REGS_S4)(a0)
	ld	s5,(REGS_S5)(a0)
	ld	s6,(REGS_S6)(a0)
	ld	s7,(REGS_S7)(a0)
	ld	s8,(REGS_S8)(a0)
	ld	s9,(REGS_S9)(a0)
	ld	s10,(REGS_S10)(a0)
	ld	s11,(REGS_S11)(a0)
	ld	t3,(REGS_T3)(a0)
	ld	t4,(REGS_T4)(a0)
	ld	t5,(REGS_T5)(a0)
	ld	t6,(REGS_T6)(a0)

	csrw	mstatus,0
	csrw	mscratch,a0
	ld	a0,(REGS_A0)(a0)

	mret

.balign 8
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
	j	syscall_invcap
.option pop
