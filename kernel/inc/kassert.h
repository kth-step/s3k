#pragma once
#include "kprintf.h"

/**
 * @file kassert.h
 *
 * @brief Assertion support for the kernel.
 *
 * This header provides support for assertions in the kernel. When the NDEBUG
 * symbol is not defined, the `kassert()` macro can be used to check whether a
 * condition is true. If the condition is false, the `kassert_failure()`
 * function will be called to report the failure. If NDEBUG is defined, then
 * if the condition is false, we get a __builtin_unreachable, hinting to the
 * compiler that the condition is assumed to be true.
 *
 * @copyright MIT License
 */
#ifndef NDEBUG

#define KASSERT_FAILURE(FILE, LINE) \
	kprintf("Kernel assertion failed at %s:%d.\n", FILE, LINE);

#define KASSERT(EXPR)                                        \
	do {                                                 \
		if (!(EXPR)) {                               \
			KASSERT_FAILURE(__FILE__, __LINE__); \
			while (1)                            \
				;                            \
		}                                            \
	} while (false)

#else /* NDEBUG */

#define KASSERT(expr)                            \
	do {                                     \
		if (!(expr)) {                   \
			__builtin_unreachable(); \
			while (1)                \
				;                \
		}                                \
	} while (false)

#endif /* NDEBUG */
