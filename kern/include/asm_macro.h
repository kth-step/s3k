#ifndef OFFSETS_H
#define OFFSETS_H

#if __riscv_xlen == 32
#define _X(x, y) x
#elif __riscv_xlen == 64
#define _X(x, y) y
#else
#error "Unsupported RISC-V architecture. Only 32-bit and 64-bit are supported."
#endif

#define OFFSET_SIZE _X(4, 8) ///< Offset size for 32-bit and 64-bit architectures.
#define LREG _X(lw, ld)	     ///< Load register instruction for 32-bit and 64-bit architectures.
#define SREG _X(sw, sd)	     ///< Store register instruction for 32-bit and 64-bit architectures.

// Offsets for each register in the PCB.
#define PROC_STATE (OFFSET_SIZE * 0) ///< Offset for the process state.
#define PROC_PC (OFFSET_SIZE * 1)    ///< Offset for the program counter (PC).
#define PROC_RA (OFFSET_SIZE * 2)    ///< Offset for the return address (RA).
#define PROC_SP (OFFSET_SIZE * 3)    ///< Offset for the stack pointer (SP).
#define PROC_GP (OFFSET_SIZE * 4)    ///< Offset for the global pointer (GP).
#define PROC_TP (OFFSET_SIZE * 5)    ///< Offset for the thread pointer (TP).
#define PROC_A0 (OFFSET_SIZE * 6)    ///< Offset for argument register A0.
#define PROC_A1 (OFFSET_SIZE * 7)    ///< Offset for argument register A1.
#define PROC_A2 (OFFSET_SIZE * 8)    ///< Offset for argument register A2.
#define PROC_A3 (OFFSET_SIZE * 9)    ///< Offset for argument register A3.
#define PROC_A4 (OFFSET_SIZE * 10)   ///< Offset for argument register A4.
#define PROC_A5 (OFFSET_SIZE * 11)   ///< Offset for argument register A5.
#define PROC_A6 (OFFSET_SIZE * 12)   ///< Offset for argument register A6.
#define PROC_A7 (OFFSET_SIZE * 13)   ///< Offset for argument register A7.
#define PROC_T0 (OFFSET_SIZE * 14)   ///< Offset for temporary register T0.
#define PROC_T1 (OFFSET_SIZE * 15)   ///< Offset for temporary register T1.
#define PROC_T2 (OFFSET_SIZE * 16)   ///< Offset for temporary register T2.
#define PROC_T3 (OFFSET_SIZE * 17)   ///< Offset for temporary register T3.
#define PROC_T4 (OFFSET_SIZE * 18)   ///< Offset for temporary register T4.
#define PROC_T5 (OFFSET_SIZE * 19)   ///< Offset for temporary register T5.
#define PROC_T6 (OFFSET_SIZE * 20)   ///< Offset for temporary register T6.
#define PROC_S0 (OFFSET_SIZE * 21)   ///< Offset for saved register S0.
#define PROC_S1 (OFFSET_SIZE * 22)   ///< Offset for saved register S1.
#define PROC_S2 (OFFSET_SIZE * 23)   ///< Offset for saved register S2.
#define PROC_S3 (OFFSET_SIZE * 24)   ///< Offset for saved register S3.
#define PROC_S4 (OFFSET_SIZE * 25)   ///< Offset for saved register S4.
#define PROC_S5 (OFFSET_SIZE * 26)   ///< Offset for saved register S5.
#define PROC_S6 (OFFSET_SIZE * 27)   ///< Offset for saved register S6.
#define PROC_S7 (OFFSET_SIZE * 28)   ///< Offset for saved register S7.
#define PROC_S8 (OFFSET_SIZE * 29)   ///< Offset for saved register S8.
#define PROC_S9 (OFFSET_SIZE * 30)   ///< Offset for saved register S9.
#define PROC_S10 (OFFSET_SIZE * 31)  ///< Offset for saved register S10.
#define PROC_S11 (OFFSET_SIZE * 32)  ///< Offset for saved register S11.

// Offsets for PMP (Physical Memory Protection) configuration in the PCB.
#define PROC_PMPADDR0 (OFFSET_SIZE * 33) ///< Offset for the first PMP address register.
#define PROC_PMPADDR1 (OFFSET_SIZE * 34) ///< Offset for the second PMP address register.
#define PROC_PMPADDR2 (OFFSET_SIZE * 35) ///< Offset for the third PMP address register.
#define PROC_PMPADDR3 (OFFSET_SIZE * 36) ///< Offset for the fourth PMP address register.
#define PROC_PMPADDR4 (OFFSET_SIZE * 37) ///< Offset for the fifth PMP address register.
#define PROC_PMPADDR5 (OFFSET_SIZE * 38) ///< Offset for the sixth PMP address register.
#define PROC_PMPADDR6 (OFFSET_SIZE * 39) ///< Offset for the seventh PMP address register.
#define PROC_PMPADDR7 (OFFSET_SIZE * 40) ///< Offset for the eighth PMP address register.
#define PROC_PMPCFG0 (OFFSET_SIZE * 41)	 ///< Offset for the first PMP configuration register.
#define PROC_PMPCFG1 (OFFSET_SIZE * 42)	 ///< Offset for the second PMP configuration register.

#endif // OFFSETS_H
