#pragma once
/**
 * @file trap.h
 * @brief Declares trap entry/exit functions.
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */
#include "kern/macro.h"

#include <stdint.h>

void trap_handler(void) NORETURN;
void trap_resume(proc_t *) NORETURN;
void trap_return(proc_t *) NORETURN;
