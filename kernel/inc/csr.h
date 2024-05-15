/**
 * @file csr.h
 * @brief Interact with control and status registers.
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */
#pragma once

/* Machine CSR constants */
#define MIP_MSIP 0x8
#define MIE_MSIE 0x8
#define MIP_MTIP 0x80
#define MIE_MTIE 0x80
#define MCAUSE_USER_ECALL 0x8
#define MSTATUS_MIE 0x8

#ifndef __ASSEMBLER__
#include <stdint.h>
uint64_t csrr_mhartid(void);
uint64_t csrr_mip(void);
uint64_t csrr_mcycle(void);
uint64_t csrr_mhpmcounter3(void);
void csrw_mcycle(uint64_t val);
void csrw_mhpmcounter3(uint64_t val);
void csrw_mstatus(uint64_t val);
void csrs_mstatus(uint64_t val);
void csrc_mstatus(uint64_t val);
uint64_t csrr_pmpcfg0(void);
uint64_t csrr_pmpaddr0(void);
uint64_t csrr_pmpaddr1(void);
uint64_t csrr_pmpaddr2(void);
uint64_t csrr_pmpaddr3(void);
uint64_t csrr_pmpaddr4(void);
uint64_t csrr_pmpaddr5(void);
uint64_t csrr_pmpaddr6(void);
uint64_t csrr_pmpaddr7(void);
void csrw_pmpcfg0(uint64_t val);
void csrw_pmpaddr0(uint64_t val);
void csrw_pmpaddr1(uint64_t val);
void csrw_pmpaddr2(uint64_t val);
void csrw_pmpaddr3(uint64_t val);
void csrw_pmpaddr4(uint64_t val);
void csrw_pmpaddr5(uint64_t val);
void csrw_pmpaddr6(uint64_t val);
void csrw_pmpaddr7(uint64_t val);
#endif
