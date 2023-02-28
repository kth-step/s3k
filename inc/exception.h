/**
 * @file exception.h
 * @brief User exception handler
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */

#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#include <stdint.h>
void handle_exception(uint64_t mcause, uint64_t mepc, uint64_t mtval);

#endif /* __EXCEPTION_H__ */
