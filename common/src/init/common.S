.globl memset
.globl memcpy
.type memset,@function
.type memcpy,@function

memset:
	beqz	a2,2f
	mv	t0,a0
	add	a2,a2,a0
1:	sb	a1,(t0)
	addi	t0,t0,1
	bne	t0,a2,1b
2:	ret

memcpy:
	beqz	a2,2f
	mv	t0,a0
	add	a2,a2,a0
1:	lb	t1,(a1)
	addi	a1,a1,1
	addi	t0,t0,1
	sb	t1,-1(t0)
	bne	t0,a2,1b
2:	ret
