.globl memset
.type memset,@function

.section .text.libaltc

// void *memset(void *s, int c, size_t n);
memset:
	beqz	a2,2f		// if (n == 0) goto exit;
	mv	t0,a0		// char *t0 = s;
	add	a2,a2,a0	// char *a2 = s + n;
				// do {
1:	sb	a1,(t0)		// 	*t0 = c;
	addi	t0,t0,1		// 	++t0;
	bne	t0,a2,1b	// } while (t0 != a2);
2:	ret			// exit: return s;
