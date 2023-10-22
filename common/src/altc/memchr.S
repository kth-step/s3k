.globl memchr
.type memchr,@function

.section .text.libaltc

// void *memchr(const void *s, int c, size_t n);
memchr:
	beqz	a2,2f	  	// if (n == 0) goto exit1;
	add	a2,a2,a0  	// char *a2 = s + n;
	andi	a1,a1,0xFF	// c = c & 0xFF;
				// do {
1:	lb	t0,(a0)		// 	t0 = *s;
	addi	a0,a0,1   	// 	++s;
	beq	t0,a1,3f  	// 	if (t0 == c) goto exit2;
	bne	a0,a1,1b  	// } while (s != a2);
2:	li	a0,0		// exit1: return NULL;
3:	ret			// exit2: return s;
