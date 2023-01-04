/* See LICENSE file for copyright and license details. */
#ifndef __STACK_H__
#define __STACK_H__

#define LOG_STACK_SIZE 8
#define STACK_SIZE     (1ull << LOG_STACK_SIZE)

#ifndef __ASSEMBLER__
char stack[NHART][STACK_SIZE]
#else
// clang-format off
.extern stack
.extern stack_top
.macro load_sp tmp
	la	sp,stack_top
	csrr	\tmp,mhartid
	slli	\tmp,\tmp,LOG_STACK_SIZE
	sub	sp,sp,\tmp
.endm
// clang-format on
#endif /* __ASSEMBLER__ */

#endif /* __STACK_H__ */
