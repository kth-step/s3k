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
#define csrr(__reg)                                                \
	({                                                         \
		unsigned long __ret;                               \
		__asm__ volatile("csrr %0," #__reg : "=r"(__ret)); \
		__ret;                                             \
	})
#define csrw(__reg, __val) \
	({ __asm__ volatile("csrw " #__reg ", %0" ::"r"(__val)); })
#define csrrw(__reg, __val)                               \
	({                                                \
		unsigned long __ret;                      \
		__asm__ volatile("csrrw %0," #__reg ",%1" \
				 : "=r"(__ret)            \
				 : "r"(__val));           \
		__ret;                                    \
	})
#define csrs(__reg, __val) \
	({ __asm__ volatile("csrs " #__reg ", %0" ::"r"(__val)); })
#define csrc(__reg, __val) \
	({ __asm__ volatile("csrc " #__reg ", %0" ::"r"(__val)); })
#endif
