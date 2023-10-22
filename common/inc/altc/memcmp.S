.globl memcmp
.type memcmp,@function

.section .text.libaltc

// int memcmp(const void *s1, const void *s2, size_t n);
memcmp:
	beqz	a2,2f		// if (n == 0) goto eq;
	add	a2,a2,a0	// char *a2 = s + n;
				// do {
1:	lb	t0,(a0)		// 	char t0 = *s1;
	lb	t1,(a1)		// 	char t1 = *s2;
	addi	a0,a0,1		// 	++s1;
	addi	a1,a1,1		// 	++s2;
	bne	t0,t1,3f	// 	if (t0 != t1) goto neq;
	bne	a0,a2,1b	// } while (s1 != a2);
2:	li	a0,1		// eq: return 1;
	ret
3:	li	a0,0		// neq: return 0;
	ret
