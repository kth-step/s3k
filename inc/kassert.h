/**
 * @file common.h
 * @brief Macros.
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */

#ifndef __KASSERT_H__
#define __KASSERT_H__

#ifndef NDEBUG

/* If debug */
#include "altio.h"
#include "common.h"
#include "csr.h"
#define kassert(x)                                                       \
	if (!(x)) {                                                      \
		alt_printf("s3k(0x%X): %s:%s Assertion `%s' failed.",    \
			   csrr_mhartid(), __FILE__, STR(__LINE__), \
			   STR(x));                                      \
		while (1)                                                \
			;                                                \
	}
#else

/* If no debug */
#define kassert(x) \
	if (!(x))  \
	__builtin_unreachable()

#endif /* NDEBUG */

#endif /* __KASSERT_H__ */
