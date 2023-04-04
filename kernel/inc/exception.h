#pragma once
/**
 * @file exception.h
 * @brief User exception handler
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */

#include "proc.h"

#include <stdint.h>

/**
 * @brief Handle an exception
 *
 * This function handles exceptions by checking the mcause register for
 * specific types of exceptions. If the exception is an illegal instruction
 * and the mtval register contains a valid return instruction, it calls the
 * handle_ret() function. Otherwise, it calls the handle_default() function
 * to handle the exception.
 *
 * @param mcause  The value of the mcause register
 * @param mepc  The value of the mepc register
 * @param mtval  The value of the mtval register
 */
proc_t *handle_exception(proc_t *p, reg_t mcause, reg_t mepc, reg_t mtval);
