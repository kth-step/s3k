/**
 * @file common.h
 * @brief Macros.
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */
#pragma once

#ifndef NDEBUG
/* Check if property holds, if not, halt */
#define assert(x)    \
	while (!(x)) \
		__asm__ volatile("wfi");
#else
/* Assume the property holds */
#define assert(x) \
	if (!(x)) \
	__builtin_unreachable()
#endif

/* Do not inline the function. */
#define NOINLINE __attribute__((noinline))
/* The function is side-effect free but may depend on the environment. */
#define PURE	 __attribute__((pure))
/* The function is side-effect free and does not depend on mutable environment. */
#define CONST	 __attribute__((const))

/* Returns the size of an array */
#define ARRAY_SIZE(x) (sizeof(x) / (sizeof((x)[0])))
