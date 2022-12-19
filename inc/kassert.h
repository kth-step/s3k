// See LICENSE file for copyright and license details.
#pragma once

#ifndef NDEBUG
#include "kprint.h"
extern void hang(void) __attribute__((noreturn));
#define kassert(val)                                                                           \
        do {                                                                                   \
                if (!(val)) {                                                                  \
                        kprintf("{[ASSERT '%s' FAILED AT %s:%d]}\r\n", #val, __FILE__, __LINE__); \
                        hang();                                                                \
                }                                                                              \
        } while (0)
#else
#define kassert(val)                             \
        ({                                       \
                if (!(val))                      \
                        __builtin_unreachable(); \
        })
#endif
