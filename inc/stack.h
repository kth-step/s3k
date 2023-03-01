/**
 * @file stack.h
 * @brief Kernel stack definition.
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */
#ifndef __STACK_H__
#define __STACK_H__

/// Stack size of 1 KiB
#define LOG_STACK_SIZE 10
#define STACK_SIZE (1ull << LOG_STACK_SIZE)

// clang-format off
#ifdef __ASSEMBLER__
.extern stack_top
.macro load_sp tmp
	la	sp,stack_top
	csrr	\tmp,mhartid
	slli	\tmp,\tmp,LOG_STACK_SIZE
	sub	sp,sp,\tmp
.endm
#endif /* __ASSEMBLER__ */
#endif /* __STACK_H__ */
