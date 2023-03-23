/**
 * @file common.h
 * @brief Macros.
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */

#ifndef __ASSERT_H__
#define __ASSERT_H__
#include "altio.h"
#include "common.h"
#include "csr.h"

#ifndef assert
#ifndef NDEBUG
/* Check if property holds, if not, halt */
#define assert(x)                                                        \
	if (!(x)) {                                                      \
		alt_printf("s3k(0x%X): %s:%s Assertion `%s' failed.",    \
			   csrr_mhartid(), __FILE_NAME__, STR(__LINE__), \
			   STR(x));                                      \
		while (1)                                                \
			;                                                \
	}
#else
/* Assume the property holds */
#define assert(x) \
	if (!(x)) \
	__builtin_unreachable()
#endif /* NDEBUG */
#endif /* assert */

#endif /* __ASSERT_H__ */
