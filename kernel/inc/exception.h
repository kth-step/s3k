#pragma once
/**
 * @file exception.h
 * @brief User exception handler
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */

#include "proc.h"

#include <stdint.h>

proc_t *handle_exception(void);
proc_t *handle_illegal_instruction(void);
