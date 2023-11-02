.globl memcpy
.type memcpy,@function

.section .text.libaltc

// void *memcpy(void *dst, const void *src, size_t n);
memcpy:
	beqz	a2,2f		// if (n == 0) goto exit;
	mv	t0,a0		// t0 = dst;
	add	a2,a2,a0	// a2 = dst + n;
				// do {
1:	lb	t1,(a1)		// 	t1 = *src;
	addi	a1,a1,1		// 	++src;
	addi	t0,t0,1		// 	++t0;
	sb	t1,-1(t0)	// 	*(t0-1) = t1;
	bne	t0,a2,1b	// } while (t0 != a2);
2:	ret			// exit: return dst;
