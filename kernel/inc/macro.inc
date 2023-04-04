#pragma once

// See LICENSE file for copyright and license details.
.macro load_sp tmp
	la	sp,_sp
	csrr	\tmp,mhartid
#if MIN_HARTID != 0
	addi	\tmp,\tmp,-MIN_HARTID
#endif
	slli	\tmp,\tmp,LOG_STACK_SIZE
	sub	sp,sp,\tmp
.endm

.macro load_gp
.option push
.option norelax
	la	gp, __global_pointer$
.option pop
.endm
