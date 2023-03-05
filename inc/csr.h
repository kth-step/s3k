/**
 * @file csr.h
 * @brief Interact with platform
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */
#ifndef __CSR_H__
#define __CSR_H__

#include <stdint.h>

uint64_t csrr_mhartid(void);
uint64_t csrr_mip(void);
uint64_t csrr_pmpcfg0(void);
void csrw_pmpcfg0(uint64_t val);
uint64_t csrr_pmpaddr0(void);
uint64_t csrr_pmpaddr1(void);
uint64_t csrr_pmpaddr2(void);
uint64_t csrr_pmpaddr3(void);
uint64_t csrr_pmpaddr4(void);
uint64_t csrr_pmpaddr5(void);
uint64_t csrr_pmpaddr6(void);
uint64_t csrr_pmpaddr7(void);
void csrw_pmpaddr0(uint64_t val);
void csrw_pmpaddr1(uint64_t val);
void csrw_pmpaddr2(uint64_t val);
void csrw_pmpaddr3(uint64_t val);
void csrw_pmpaddr4(uint64_t val);
void csrw_pmpaddr5(uint64_t val);
void csrw_pmpaddr6(uint64_t val);
void csrw_pmpaddr7(uint64_t val);

#endif /* __CSR_H__ */
