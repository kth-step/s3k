// See LICENSE file for copyright and license details.
#include "stack.h"
.extern stack_top
.macro load_sp tmp
	la	sp,stack_top
	csrr	\tmp,mhartid
	slli	\tmp,\tmp,LOG_STACK_SIZE
	sub	sp,sp,\tmp
.endm
