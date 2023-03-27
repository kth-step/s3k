/**
 * @file init.h
 * @brief Declares the initialization function for the kernel.
 *
 * This file contains the declaration of the `init_kernel()` function, which
 * initializes the kernel with the given payload and starts the boot process.
 * The payload parameter should be a pointer to the boot process's code, which
 * will be executed by the kernel.
 *
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */
#ifndef __INIT_H__
#define __INIT_H__

#include "proc.h"

/**
 * @brief Initializes the kernel with the given payload, which is a pointer to
 * the boot process's code.
 *
 * This function sets up the kernel's and the boot processes' initial state.
 * The payload parameter should be a pointer to the boot process's code, which
 * will be the first process to execute.
 *
 * @param payload A pointer to the boot process's code.
 */
void init_kernel(uint64_t payload);

#endif /* __INIT_H__ */
