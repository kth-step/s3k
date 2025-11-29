#include "asm_macro.h"		// Include assembly macros for register offsets and other utilities.

.extern exception_handler  	// External handler for exceptions.
.extern interrupt_handler  	// External handler for interrupts.
.extern syscall_handler    	// External handler for system calls.
.extern scheduler          	// External function for scheduling processes.

.globl trap_entry  		// Make trap_entry globally accessible.
.globl trap_exit   		// Make trap_exit globally accessible.
.globl trap_resume 		// Make trap_resume globally accessible.
.type  trap_entry, @function
.type  trap_exit, @function
.type  trap_resume, @function

.section .text

// Align the trap_entry function to a 16-byte boundary.
.balign 16
trap_entry:
	csrrw	tp,mscratch,tp		// Swap the current PCB pointer with mscratch.

	SREG	ra,PROC_RA(tp)		// Save return address.
	SREG	sp,PROC_SP(tp)		// Save stack pointer.
	SREG	gp,PROC_GP(tp)		// Save global pointer.
	SREG	a0,PROC_A0(tp)		// Save register a0.
	SREG	a1,PROC_A1(tp)		// Save register a1.
	SREG	a2,PROC_A2(tp)		// Save register a2.
	SREG	a3,PROC_A3(tp)		// Save register a3.
	SREG	a4,PROC_A4(tp)		// Save register a4.
	SREG	a5,PROC_A5(tp)		// Save register a5.
	SREG	a6,PROC_A6(tp)		// Save register a6.
	SREG	a7,PROC_A7(tp)		// Save register a7.
	SREG	t0,PROC_T0(tp)		// Save register t0.
	SREG	t1,PROC_T1(tp)		// Save register t1.
	SREG	t2,PROC_T2(tp)		// Save register t2.
	SREG	t3,PROC_T3(tp)		// Save register t3.
	SREG	t4,PROC_T4(tp)		// Save register t4.
	SREG	t5,PROC_T5(tp)		// Save register t5.
	SREG	t6,PROC_T6(tp)		// Save register t6.
	SREG	s0,PROC_S0(tp)		// Save register s0.
	SREG	s1,PROC_S1(tp)		// Save register s1.
	SREG	s2,PROC_S2(tp)		// Save register s2.
	SREG	s3,PROC_S3(tp)		// Save register s3.
	SREG	s4,PROC_S4(tp)		// Save register s4.
	SREG	s5,PROC_S5(tp)		// Save register s5.
	SREG	s6,PROC_S6(tp)		// Save register s6.
	SREG	s7,PROC_S7(tp)		// Save register s7.
	SREG	s8,PROC_S8(tp)		// Save register s8.
	SREG	s9,PROC_S9(tp)		// Save register s9.
	SREG	s10,PROC_S10(tp)	// Save register s10.
	SREG	s11,PROC_S11(tp)	// Save register s11.

	// Save program counter into the PCB.
	csrr	t0,mepc			// Load the program counter of the trapped instruction.
	SREG	t0,PROC_PC(tp)		// Save program counter.

	csrr	t0,mscratch		// Get user tp.
	SREG	t0,PROC_TP(tp)		// Save register tp.


	// Set up global and stack pointers for the kernel.
	.option push
	.option norelax
	la	gp,__global_pointer$	// Load the global pointer.
	.option pop
	la	sp,__stack_top		// Load the kernel stack top.
#ifdef SMP
	csrr	t0,mhartid
	slli	t0,t0,10
	add	sp,sp,t0
#endif

_trap_dispatch:
	// Prepare for trap dispatch.
	la	ra,_trap_switch		// Set return address to trap_switch.

	// Determine the type of trap and dispatch to the appropriate handler.
	csrr	a0,mcause		// Load the trap cause.
	li	t0,8
	beq	a0,t0,syscall_handler	// If system call, jump to syscall_handler.

	csrr	a1,mtval		// Load trap value for exception handling.
	bltz	a0,interrupt_handler	// If negative, it's an interrupt; jump to interrupt_handler.
	j	exception_handler	// Otherwise, jump to exception_handler.

_trap_switch:
	// Check if a context switch is needed.
	// If the current process (a0) is the same as the next process (tp),
	// jump directly to `trap_exit` without performing a context switch.
	beq	a0,tp,trap_exit

	// Atomically update the process state to indicate it is no longer running.
	// This ensures that the process state is updated safely in a multi-core environment.
	li	t0,~1				// Load the bitmask to clear the "busy" state.
	amoand.d.rl x0,t0,(tp)

	// If the next process (a0) is NULL, jump to `sched`.
	bnez	a0,trap_resume

	call	sched

trap_resume:
	mv	tp,a0

	LREG	s0,PROC_PMPCFG0(tp)	// Load PMP configuration.
	csrw	pmpcfg0,s0		// Write PMP configuration to pmpcfg0.

	// Load PMP configuration from the PCB.
	LREG	s0,PROC_PMPADDR0(tp)	// Load PMP address0.
	LREG	s1,PROC_PMPADDR1(tp)	// Load PMP address1.
	LREG	s2,PROC_PMPADDR2(tp)	// Load PMP address2.
	LREG	s3,PROC_PMPADDR3(tp)	// Load PMP address3.
	LREG	s4,PROC_PMPADDR4(tp)	// Load PMP address4.
	LREG	s5,PROC_PMPADDR5(tp)	// Load PMP address5.
	LREG	s6,PROC_PMPADDR6(tp)	// Load PMP address6.
	LREG	s7,PROC_PMPADDR7(tp)	// Load PMP address7.
	csrw	pmpaddr0,s0		// Write PMP address to pmpaddr0.
	csrw	pmpaddr1,s1		// Write PMP address to pmpaddr1.
	csrw	pmpaddr2,s2		// Write PMP address to pmpaddr2.
	csrw	pmpaddr3,s3		// Write PMP address to pmpaddr3.
	csrw	pmpaddr4,s4		// Write PMP address to pmpaddr4.
	csrw	pmpaddr5,s5		// Write PMP address to pmpaddr5.
	csrw	pmpaddr6,s6		// Write PMP address to pmpaddr6.
	csrw	pmpaddr7,s7		// Write PMP address to pmpaddr7.


trap_exit:
	// Restore trap context and return to the next instruction.
	LREG	t0,PROC_PC(tp)		// Restore program counter.
	csrw	mepc,t0			// Write program counter back to mepc.

	LREG	t0,PROC_TP(tp)		// Save user TP to mscratch.
	csrw	mscratch,t0
	LREG	ra,PROC_RA(tp)		// Restore return address.
	LREG	sp,PROC_SP(tp)		// Restore stack pointer.
	LREG	gp,PROC_GP(tp)		// Restore global pointer.
	LREG	a0,PROC_A0(tp)		// Restore register a0.
	LREG	a1,PROC_A1(tp)		// Restore register a1.
	LREG	a2,PROC_A2(tp)		// Restore register a2.
	LREG	a3,PROC_A3(tp)		// Restore register a3.
	LREG	a4,PROC_A4(tp)		// Restore register a4.
	LREG	a5,PROC_A5(tp)		// Restore register a5.
	LREG	a6,PROC_A6(tp)		// Restore register a6.
	LREG	a7,PROC_A7(tp)		// Restore register a7.
	LREG	t0,PROC_T0(tp)		// Restore register t0.
	LREG	t1,PROC_T1(tp)		// Restore register t1.
	LREG	t2,PROC_T2(tp)		// Restore register t2.
	LREG	t3,PROC_T3(tp)		// Restore register t3.
	LREG	t4,PROC_T4(tp)		// Restore register t4.
	LREG	t5,PROC_T5(tp)		// Restore register t5.
	LREG	t6,PROC_T6(tp)		// Restore register t6.
	LREG	s0,PROC_S0(tp)		// Restore register s0.
	LREG	s1,PROC_S1(tp)		// Restore register s1.
	LREG	s2,PROC_S2(tp)		// Restore register s2.
	LREG	s3,PROC_S3(tp)		// Restore register s3.
	LREG	s4,PROC_S4(tp)		// Restore register s4.
	LREG	s5,PROC_S5(tp)		// Restore register s5.
	LREG	s6,PROC_S6(tp)		// Restore register s6.
	LREG	s7,PROC_S7(tp)		// Restore register s7.
	LREG	s8,PROC_S8(tp)		// Restore register s8.
	LREG	s9,PROC_S9(tp)		// Restore register s9.
	LREG	s10,PROC_S10(tp)	// Restore register s10.
	LREG	s11,PROC_S11(tp)	// Restore register s11.

	csrrw	tp,mscratch,tp		// Swap PCB pointer with mscratch (user tp).
	mret				// Return from trap.
