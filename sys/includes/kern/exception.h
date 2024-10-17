#pragma once
/**
 * @file exception.h
 * @brief User exception handler
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */

#include "kern/proc.h"

#include "kern/types.h"

void exception_handler(uint64_t mcause, uint64_t mtval);
