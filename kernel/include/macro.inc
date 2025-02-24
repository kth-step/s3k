#pragma once

// See LICENSE file for copyright and license details.
.macro ld_sp tmp
	la	sp,_sp
	csrr	\tmp,mhartid
	slli	\tmp,\tmp,8
	sub	sp,sp,\tmp
.endm

.macro ld_gp
.option push
.option norelax
	la	gp, __global_pointer$
.option pop
.endm
