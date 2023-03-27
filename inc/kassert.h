/**
 * @file kassert.h
 *
 * @brief Assertion support for the kernel.
 *
 * This header provides support for assertions in the kernel. When the NDEBUG
 * symbol is not defined, the `kassert()` macro can be used to check whether a
 * condition is true. If the condition is false, the `kassert_failure()`
 * function will be called to report the failure.
 *
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */
#ifndef __KASSERT_H__
#define __KASSERT_H__

#ifdef NDEBUG

#define kassert(expr) ((void)0)

#else /* NDEBUG */

#include <stdint.h> // for uint64_t type

/**
 * @brief Assertion failure handler.
 *
 * This function is called when an assertion fails. It prints an error message
 * to the console using the `alt_printf()` function and halts the system.
 *
 * @param file The name of the source file where the assertion failed.
 * @param line The line number in the source file where the assertion failed.
 * @param expr The assertion expression that failed.
 */
void kassert_failure(const char *file, uint64_t line, const char *expr);

/**
 * @brief Asserts that an expression is true.
 *
 * This macro evaluates the given expression and, if it is false, calls the
 * `kassert_failure()` function with information about the assertion failure.
 *
 * @param expr The expression to evaluate.
 */
#define kassert(expr)                                               \
	do {                                                        \
		if (!(expr)) {                                      \
			kassert_failure(__FILE__, __LINE__, #expr); \
		}                                                   \
	} while (false)

#endif /* NDEBUG */

#endif /* __KASSERT_H__ */
