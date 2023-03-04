/**
 * @file platform.h
 * @brief Interact with platform
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */
#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include <stdint.h>

uint64_t time_get(void);
uint64_t timeout_get(uint64_t i);
void timeout_set(uint64_t i, uint64_t val);
uint64_t csrr_mhartid(void);
uint64_t csrr_mip(void);
uint64_t csrr_pmpcfg(uint64_t i);
void csrw_pmpcfg(uint64_t i, uint64_t val);
uint64_t csrr_pmpaddr(uint64_t i);
void csrw_pmpaddr(uint64_t i, uint64_t val);

#endif /* __PLATFORM_H__ */
