#pragma once
/**
 * @file exception.h
 * @brief User exception handler
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */

#include "kern/proc.h"
#include "kern/types.h"

Proc *ExceptionHandler(Proc *proc, Word mcause, Word mtval);
