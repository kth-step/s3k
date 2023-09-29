#pragma once
/**
 * @file trap.h
 * @brief Declares trap entry/exit functions.
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */
#include <stdint.h>

void trap_entry(void) __attribute__((noreturn));
void trap_exit(void) __attribute__((noreturn));
void trap_schedule_exit(void) __attribute__((noreturn));
void trap_syscall_exit(uint64_t a0) __attribute__((noreturn))
__attribute__((naked));
