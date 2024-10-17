#pragma once
/**
 * @file trap.h
 * @brief Declares trap entry/exit functions.
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */
#include "kern/macro.h"
#include "kern/proc.h"

void TrapHandler(void) NORETURN;
void TrapResume(Proc *) NORETURN;
void TrapReturn(Proc *) NORETURN;
