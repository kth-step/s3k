.globl _start

.section .text.init

_start:
	.option push
	.option norelax
	la	gp,__global_pointer$
	.option pop
	// Set up the stack pointer
	la	sp,__stack_top
	
	// Call main function
	call	main
_hang:
	// Infinite loop to hang the program
	j 	_hang
