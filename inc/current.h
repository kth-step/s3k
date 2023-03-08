/**
 * @file current.h
 * @brief Set current process
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */
#ifndef __CURRENT_H__
#define __CURRENT_H__
#include "proc.h"

void current_set(struct proc *proc);
struct proc *current_get();
#endif /* __CURRENT_H__ */
