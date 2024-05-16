#pragma once
/**
 * @file exception.h
 * @brief User exception handler
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */

#include "proc.h"

#include <stdint.h>

proc_t *exception_handler(proc_t *proc, uint64_t mcause, uint64_t mtval);
