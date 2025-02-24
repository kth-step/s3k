#pragma once
/**
 * @file trap.h
 * @brief Declares trap entry/exit functions.
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */
#include "macro.h"

#include <stdint.h>

void trap_handle(void);
void trap_entry(void) NORETURN;
void trap_resume(proc_t *proc) NORETURN;
void trap_return(proc_t *proc) NORETURN;
