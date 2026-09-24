.extern trap_entry	// Address of the trap entry handler.
.extern trap_resume	// Address of the trap resume handler.
.extern kernel_init	// Address of the kernel initialization function.

.globl _start		

.type  _start, @function	// Define the type of the _start function.

.section .text.init
// Entry point of the kernel.
_start:

	// Clear machine-mode scratch and status registers.
	csrw	mscratch,x0		// Clear the mscratch register.
	csrw	mstatus,x0		// Clear the mstatus register.
	li	t0,128
	csrw    mie,t0
	csrw	mcounteren,0xf
	csrw	mcountinhibit,0x0
#ifndef PLATFORM_NO_SCOUNTEREN
	// scounteren does not exist on M-only cores.
	csrw	scounteren,0xf
#endif

	// Set the trap vector to the trap entry handler for handling traps.
	la	t0,trap_entry		// Load the address of the trap entry handler.
	csrw	mtvec,t0		// Set the machine trap vector.

	// Set the context switch padding.
	// The context switch padding = CSPAD + (hartid * 16).
#if CSPAD != 0
	csrr	t0,mhartid
	slli	t0,t0,6
	li	t1,CSPAD
	add	t1,t1,t0
	csrw	0x7C3,t1
#endif

	// Initialize the global pointer (gp) and stack pointer (sp).
	.option push
	.option norelax
	la	gp,__global_pointer$	// Load the global pointer.
	.option pop
	la	sp,__stack_top		// Load the stack pointer.

#ifdef SMP
	csrr	t0,mhartid
	slli	t0,t0,10
	add	sp,sp,t0
#endif

	// Mechanism so hart 0 initialize the kernel.
	// TODO: Fix this so it relies on software interrupts instead.
	// This method seems a bit buggy, especially in the wait phase.
	csrr	t0,mhartid
	li	t1,_NUM_HARTS
	bgeu	t0,t1,_hang
	bnez	t0,_wait	

#ifdef PLATFORM_PREEMPT_STK
_copy_data:
	// Copy .data from its flash LMA to the SRAM VMA (ch32v307 links code
	// in flash and writable state in SRAM; see ch32v307.ld).
	la	t0,_data		// SRAM destination (VMA).
	la	t1,_end			// End of data+bss (VMA).
	la	t2,_data_lma		// Flash source (LMA).
1:	bgeu	t0,t1,2f
	lw	t3,0(t2)
	sw	t3,0(t0)
	addi	t0,t0,4
	addi	t2,t2,4
	j	1b
2:
#endif

_zero_bss:
	// Zero out the .bss section (uninitialized global variables).
	la	t0,_bss			// Start address of the .bss section.
	la	t1,_end			// End address of the .bss section.
1:	sb	x0,0(t0)		// Store zero at the current address.
	addi	t0,t0,1			// Move to the next byte.
	bne	t0,t1,1b		// Repeat until the .bss section is cleared.

_init:
#ifdef PLATFORM_PREEMPT_STK
	// Kernel-stack watermark: fill with 0xA5 before any C frame exists.
	la	t0,_end
	la	t1,__stack_top
	li	t2,0xa5a5a5a5
1:	sw	t2,0(t0)
	addi	t0,t0,4
	bne	t0,t1,1b
#endif

	// Initialize the kernel environment.
	call	kernel_init


	// Unlock the kernel initialization lock.
	la	t1,_lock
	amoor.w x0,t1,(t1)
_wait:
	la	t1,_lock
	amoor.w   t1,x0,(t1)
	beqz	t1,_wait

	// Initialize the first process and transfer control to it.
	call	sched			// Call the scheduler to fetch the first process.
	tail	trap_resume		// Jump to the trap resume handler.

	// Harts with ID >= _NUM_HARTS should hang.
_hang:
	csrw	mie,0
	wfi
	j	_hang
	

.section .data
	// Lock used during kernel initialization.
	_lock:
	.word 0
