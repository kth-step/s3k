#pragma once
/**
 * @file trap.h
 * @brief Declares trap entry/exit functions.
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */
#include "macro.h"

#include <stdint.h>

void trap_entry(void) NORETURN;
void trap_exit(void) NORETURN;
