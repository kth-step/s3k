/* See LICENSE file for copyright and license details. */
#pragma once

#define LOG_STACK_SIZE 10
#define STACK_SIZE     (1ull << LOG_STACK_SIZE)

// clang-format off
.extern stack_top
.macro load_sp tmp
	la	sp,stack_top
	csrr	\tmp,mhartid
	slli	\tmp,\tmp,LOG_STACK_SIZE
	sub	sp,sp,\tmp
.endm
