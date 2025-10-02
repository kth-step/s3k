#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "proc.h"
#include "types.h"

/**
 * @brief Handles exceptions for a process.
 *
 * This function is called when an exception occurs in a process.
 * It processes the exception and returns the updated process control block.
 *
 * @param cause The cause of the exception.
 * @param tval The trap value associated with the exception.
 * @return The updated process control block.
 */
proc_t *exception_handler(word_t cause, word_t tval);

#endif // EXCEPTION_H
