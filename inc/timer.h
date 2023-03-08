/**
 * @file platform.h
 * @brief Interact with platform
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */
#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>

uint64_t time_get(void);
void time_set(uint64_t time);
uint64_t timeout_get(uint64_t i);
void timeout_set(uint64_t i, uint64_t val);

#endif /* __TIMER_H__ */
