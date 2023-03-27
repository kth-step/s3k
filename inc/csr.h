/**
 * @file csr.h
 * @brief Interact with platform
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */
#ifndef __CSR_H__
#define __CSR_H__

#include <stdint.h>

/**
 * @brief Read the hardware thread ID (mhartid) register.
 *
 * @return uint64_t The value of the mhartid register.
 */
uint64_t csrr_mhartid(void);

/**
 * @brief Read the machine interrupt pending register (mip).
 *
 * @return uint64_t The value of the mip register.
 */
uint64_t csrr_mip(void);

/**
 * @brief Read the PMP configuration register 0 (pmpcfg0).
 *
 * @return uint64_t The value of the pmpcfg0 register.
 */
uint64_t csrr_pmpcfg0(void);

/**
 * @brief Write to the PMP configuration register 0 (pmpcfg0).
 *
 * @param val The value to write to the pmpcfg0 register.
 */
void csrw_pmpcfg0(uint64_t val);

/**
 * @brief Read PMP address register 0.
 *
 * @return uint64_t The value of the pmpaddr0 register.
 */
uint64_t csrr_pmpaddr0(void);

/**
 * @brief Read PMP address register 1.
 *
 * @return uint64_t The value of the pmpaddr1 register.
 */
uint64_t csrr_pmpaddr1(void);

/**
 * @brief Read PMP address register 2.
 *
 * @return uint64_t The value of the pmpaddr2 register.
 */
uint64_t csrr_pmpaddr2(void);

/**
 * @brief Read PMP address register 3.
 *
 * @return uint64_t The value of the pmpaddr3 register.
 */
uint64_t csrr_pmpaddr3(void);

/**
 * @brief Read PMP address register 4.
 *
 * @return uint64_t The value of the pmpaddr4 register.
 */
uint64_t csrr_pmpaddr4(void);

/**
 * @brief Read PMP address register 5.
 *
 * @return uint64_t The value of the pmpaddr5 register.
 */
uint64_t csrr_pmpaddr5(void);

/**
 * @brief Read PMP address register 6.
 *
 * @return uint64_t The value of the pmpaddr6 register.
 */
uint64_t csrr_pmpaddr6(void);

/**
 * @brief Read PMP address register 7.
 *
 * @return uint64_t The value of the pmpaddr7 register.
 */
uint64_t csrr_pmpaddr7(void);

/**
 * @brief Write to PMP address register 0.
 *
 * @param val The value to write to the pmpaddr0 register.
 */
void csrw_pmpaddr0(uint64_t val);

/**
 * @brief Write to PMP address register 1.
 *
 * @param val The value to write to the pmpaddr1 register.
 */
void csrw_pmpaddr1(uint64_t val);

/**
 * @brief write to pmp address register 2.
 *
 * @param val the value to write to the pmpaddr2 register.
 */
void csrw_pmpaddr2(uint64_t val);

/**
 * @brief write to pmp address register 3.
 *
 * @param val the value to write to the pmpaddr3 register.
 */
void csrw_pmpaddr3(uint64_t val);

/**
 * @brief write to pmp address register 4.
 *
 * @param val the value to write to the pmpaddr4 register.
 */
void csrw_pmpaddr4(uint64_t val);

/**
 * @brief write to pmp address register 5.
 *
 * @param val the value to write to the pmpaddr4 register.
 */
void csrw_pmpaddr5(uint64_t val);

/**
 * @brief write to pmp address register 6.
 *
 * @param val the value to write to the pmpaddr4 register.
 */
void csrw_pmpaddr6(uint64_t val);

/**
 * @brief write to pmp address register 7.
 *
 * @param val the value to write to the pmpaddr4 register.
 */
void csrw_pmpaddr7(uint64_t val);

#endif /* __CSR_H__ */
