#pragma once

// See LICENSE file for copyright and license details.
.macro ld_sp tmp
	la	sp,_sp
	csrr	\tmp,mhartid
#if S3K_MIN_HART != 0
	addi	\tmp,\tmp,-S3K_MIN_HART
#endif
	slli	\tmp,\tmp,S3K_LOG_STACK_SIZE
	sub	sp,sp,\tmp
.endm

.macro ld_gp
.option push
.option norelax
	la	gp, __global_pointer$
.option pop
.endm

.macro	instr_init
#ifdef INSTRUMENT
	csrw	mcounteren,0x3
	csrw	scounteren,0x3
	csrw	mcountinhibit,0x8
	csrw	mhpmcounter3,0
#endif
.endm
