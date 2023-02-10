/* See LICENSE file for copyright and license details. */
/**
 * @file csr.h
 * @brief Functions for reading and writing to CSR registers.
 *
 * @author Henrik Karlsson (henrik10@kth.se)
 */
#pragma once
#include <stdint.h>

#include "common.h"

/** Hardware thread ID */
uint64_t csrr_mhartid(void) CONST;

/** Pending interrupt */
uint64_t csrr_mip(void);

/* Physical Memory protection */
/** Read pmpcfg0 */
uint64_t csrr_pmpcfg0(void) PURE;
/** Read pmpaddr0 */
uint64_t csrr_pmpaddr0(void) PURE;
/** Read pmpaddr1 */
uint64_t csrr_pmpaddr1(void) PURE;
/** Read pmpaddr2 */
uint64_t csrr_pmpaddr2(void) PURE;
/** Read pmpaddr3 */
uint64_t csrr_pmpaddr3(void) PURE;
/** Read pmpaddr4 */
uint64_t csrr_pmpaddr4(void) PURE;
/** Read pmpaddr5 */
uint64_t csrr_pmpaddr5(void) PURE;
/** Read pmpaddr6 */
uint64_t csrr_pmpaddr6(void) PURE;
/** Read pmpaddr7 */
uint64_t csrr_pmpaddr7(void) PURE;

/** Write to pmpcfg0 */
void csrw_pmpcfg0(uint64_t val);
/** Write to pmpaddr0 */
void csrw_pmpaddr0(uint64_t val);
/** Write to pmpaddr1 */
void csrw_pmpaddr1(uint64_t val);
/** Write to pmpaddr2 */
void csrw_pmpaddr2(uint64_t val);
/** Write to pmpaddr3 */
void csrw_pmpaddr3(uint64_t val);
/** Write to pmpaddr4 */
void csrw_pmpaddr4(uint64_t val);
/** Write to pmpaddr5 */
void csrw_pmpaddr5(uint64_t val);
/** Write to pmpaddr6 */
void csrw_pmpaddr6(uint64_t val);
/** Write to pmpaddr7 */
void csrw_pmpaddr7(uint64_t val);
