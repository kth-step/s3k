#ifndef __CSR_H__
#define __CSR_H__
#include <stdint.h>

#include "common.h"

/* Hardware thread ID */
uint64_t csrr_mhartid(void) CONST;

/* Physical Memory protection */
uint64_t csrr_pmpcfg0(void) PURE;
uint64_t csrr_pmpaddr0(void) PURE;
uint64_t csrr_pmpaddr1(void) PURE;
uint64_t csrr_pmpaddr2(void) PURE;
uint64_t csrr_pmpaddr3(void) PURE;
uint64_t csrr_pmpaddr4(void) PURE;
uint64_t csrr_pmpaddr5(void) PURE;
uint64_t csrr_pmpaddr6(void) PURE;
uint64_t csrr_pmpaddr7(void) PURE;

void csrw_pmpcfg0(uint64_t val);
void csrw_pmpaddr0(uint64_t val);
void csrw_pmpaddr1(uint64_t val);
void csrw_pmpaddr2(uint64_t val);
void csrw_pmpaddr3(uint64_t val);
void csrw_pmpaddr4(uint64_t val);
void csrw_pmpaddr5(uint64_t val);
void csrw_pmpaddr6(uint64_t val);
void csrw_pmpaddr7(uint64_t val);

#endif /* __CSR_H__ */
