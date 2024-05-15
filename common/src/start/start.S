.globl _start
#define STACK_SIZE 1024
/* Defines program entry */
.section .init,"ax",@progbits
_start:

.option push
.option norelax
	la gp,__global_pointer$
.option pop
	la sp,__stack_top

	la t0,_bss
	la t1,_end
	j 2f
1:	sb zero,(t0)
	addi t0,t0,1
2:	bne t0,t1,1b

	call main

1:	j 1b

/* Defines stack */
.section .bss,"aw",@nobits
.balign 8
	.space STACK_SIZE
__stack_top:
