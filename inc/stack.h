/**
 * @file stack.h
 * @brief Kernel stack definition.
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */
#ifndef __STACK_H__
#define __STACK_H__
#include "platform.h"

/// Stack size of 1 KiB
#define LOG_STACK_SIZE 10
#define STACK_SIZE (1ull << LOG_STACK_SIZE)

#endif /* __STACK_H__ */
