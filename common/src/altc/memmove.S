.globl memmove
.type memmove,@function

.section .text.libaltc

// void *memmove(void *dst, const void *src, size_t n);
memmove:
	beq	a0,a1,2f 	// if (dst == src) goto exit;
	beqz	a2,2f		// if (n == 0) goto exit;
	bltu	a0,a1,memcpy	// if (dst < src) memcpy(dst, src, n);
	add	a2,a2,a0	// a2 = dst + n;
	add	a1,a1,a0	// src = src + n;
				// do {
1:	lb	t1,(a1)		//	t1 = *src;
	addi	a1,a1,-1	//	--src;
	addi	a2,a2,-1	//	--a2;
	sb	t1,1(a2)	//	*(a2+1) = t1;
	bne	a2,a0,1b	// } while (a2 != dst);
2:	ret			// exit: return dst
