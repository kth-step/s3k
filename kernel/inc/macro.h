#pragma once
/**
 * @file macro.h
 * @brief Macros.
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */

#define _STR_(x) #x
#define STR(x) _STR_(x)

/* Do not inline the function. */
#define NOINLINE __attribute__((noinline))
/* The function is side-effect free but may depend on the environment. */
#define PURE __attribute__((pure))
/* The function is side-effect free and does not depend on mutable environment.
 */
#define CONST __attribute__((const))
#define UNREACHABLE __builtin_unreachable
#define NORETURN __attribute__((noreturn))

/* Returns the size of an array */
#define ARRAY_SIZE(x) (sizeof(x) / (sizeof((x)[0])))
