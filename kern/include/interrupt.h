#pragma once

#include "proc.h"
#include "types.h"

/**
 * @brief Handles hardware interrupts.
 *
 * This function is called when a hardware interrupt occurs.
 * It processes the interrupt and returns the updated process control block.
 *
 * @param cause The cause of the interrupt.
 * @param tval The trap value associated with the interrupt.
 * @return The updated process control block.
 */
proc_t *interrupt_handler(word_t cause, word_t tval);
