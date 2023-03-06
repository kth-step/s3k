/**
 * @file trap.h
 * @brief Declares trap entry/exit functions.
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */
#ifndef __TRAP_H__
#define __TRAP_H__
#include "common.h"

void trap_entry(void) __attribute__((noreturn));
void trap_exit(void) __attribute__((noreturn));

#endif /* __TRAP_H__ */
