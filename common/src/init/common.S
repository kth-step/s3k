.globl memset

.section .text
memset:
	beqz a2, 2f
	add a2,a2,a0
1:	sb a1,0(a0)
	addi a0,a0,1
	bne a0,a2,1b
2:	ret
