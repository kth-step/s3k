#ifndef __COMMON_H__
#define __COMMON_H__

#if DEBUG
/* Check if property holds, if not, halt */
/* TODO: print error */
#define ASSERT(x)         \
	if (!(x))         \
		while (1) \
			;
#else
/* Assume the property holds */
#define ASSERT(x) \
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

#endif /* __COMMON_H__ */
