/**
 * @file current.h
 * @brief Set current process
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */
#ifndef __CURRENT_H__
#define __CURRENT_H__
#include "proc.h"

/**
 * @brief Sets the currently executing process to the given process.
 *
 * @param proc A pointer to the process to set as the currently executing
 * process.
 */
void current_set(struct proc *proc);

/**
 * @brief Gets a pointer to the currently executing process.
 *
 * @return A pointer to the currently executing process.
 */
struct proc *current_get();

#endif /* __CURRENT_H__ */
