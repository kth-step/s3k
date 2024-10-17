#pragma once
/**
 * @file csr.h
 * @brief Interact with control and status registers.
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */

/* Machine CSR constants */
#define MIP_MSIP 0x8
#define MIE_MSIE 0x8
#define MIP_MTIP 0x80
#define MIE_MTIE 0x80
#define MCAUSE_USER_ECALL 0x8
#define MSTATUS_MIE 0x8

#ifndef __ASSEMBLER__
#define CsrRead(__reg)                                             \
	({                                                         \
		unsigned long __ret;                               \
		__asm__ volatile("csrr %0," #__reg : "=r"(__ret)); \
		__ret;                                             \
	})
#define CsrWrite(__reg, __val) ({ __asm__ volatile("csrw " #__reg ", %0" ::"r"(__val)); })
#define CsrSwap(__reg, __val)                                                          \
	({                                                                             \
		unsigned long __ret;                                                   \
		__asm__ volatile("csrrw %0," #__reg ",%1" : "=r"(__ret) : "r"(__val)); \
		__ret;                                                                 \
	})
#define CsrSet(__reg, __val) ({ __asm__ volatile("csrs " #__reg ", %0" ::"r"(__val)); })
#define CsrClear(__reg, __val) ({ __asm__ volatile("csrc " #__reg ", %0" ::"r"(__val)); })
#endif
