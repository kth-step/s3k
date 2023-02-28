/**
 * @file timer.h
 * @brief Read and set time and timeout values.
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */
#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>

/**
 * Get the current RTC time.
 *
 * @return The current value of the real-time clock.
 */
uint64_t time_get(void);
/**
 * Set the current RTC time.
 *
 * @param time New time.
 */
void time_set(uint64_t time);
/**
 * Get the timer deadline for hart `i`
 *
 * @param i Hart ID.
 * @return Deadline of hart `i`'s timer.
 */
uint64_t timeout_get(uint64_t i);
/**
 * Set the timer for hart `i`.
 *
 * @param i Hart ID.
 * @param deadline New deadline.
 */
void timeout_set(uint64_t i, uint64_t timeout);

#endif /* __TIMER_H__ */
