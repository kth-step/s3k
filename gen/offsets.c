// See LICENSE file for copyright and license details.

#include "proc.h"
#include "cap_node.h"

#define DEFINE_OFFSET(name, struct, member) __asm__ volatile ("#define " #name " %0" ::"i"(offsetof(struct, member)));
#define DEFINE_ENUM(name) __asm__ volatile ("#define " #name " %0" ::"i"(name));

void foo(void)
{
    DEFINE_OFFSET(REGS_PC, regs_t, pc);
    DEFINE_OFFSET(REGS_RA, regs_t, ra);
    DEFINE_OFFSET(REGS_SP, regs_t, sp);
    DEFINE_OFFSET(REGS_GP, regs_t, gp);
    DEFINE_OFFSET(REGS_TP, regs_t, tp);
    DEFINE_OFFSET(REGS_T0, regs_t, t0);
    DEFINE_OFFSET(REGS_T1, regs_t, t1);
    DEFINE_OFFSET(REGS_T2, regs_t, t2);
    DEFINE_OFFSET(REGS_T3, regs_t, t3);
    DEFINE_OFFSET(REGS_T4, regs_t, t4);
    DEFINE_OFFSET(REGS_T5, regs_t, t5);
    DEFINE_OFFSET(REGS_T6, regs_t, t6);
    DEFINE_OFFSET(REGS_A0, regs_t, a0);
    DEFINE_OFFSET(REGS_A1, regs_t, a1);
    DEFINE_OFFSET(REGS_A2, regs_t, a2);
    DEFINE_OFFSET(REGS_A3, regs_t, a3);
    DEFINE_OFFSET(REGS_A4, regs_t, a4);
    DEFINE_OFFSET(REGS_A5, regs_t, a5);
    DEFINE_OFFSET(REGS_A6, regs_t, a6);
    DEFINE_OFFSET(REGS_A7, regs_t, a7);
    DEFINE_OFFSET(REGS_S0, regs_t, s0);
    DEFINE_OFFSET(REGS_S1, regs_t, s1);
    DEFINE_OFFSET(REGS_S2, regs_t, s2);
    DEFINE_OFFSET(REGS_S3, regs_t, s3);
    DEFINE_OFFSET(REGS_S4, regs_t, s4);
    DEFINE_OFFSET(REGS_S5, regs_t, s5);
    DEFINE_OFFSET(REGS_S6, regs_t, s6);
    DEFINE_OFFSET(REGS_S7, regs_t, s7);
    DEFINE_OFFSET(REGS_S8, regs_t, s8);
    DEFINE_OFFSET(REGS_S9, regs_t, s9);
    DEFINE_OFFSET(REGS_S10, regs_t, s10);
    DEFINE_OFFSET(REGS_S11, regs_t, s11);
    DEFINE_OFFSET(REGS_PMP, regs_t, pmp);
    DEFINE_OFFSET(REGS_TIMEOUT, regs_t, timeout);
    DEFINE_OFFSET(REGS_TPC, regs_t, tpc);
    DEFINE_OFFSET(REGS_TSP, regs_t, tsp);
    DEFINE_OFFSET(REGS_CAUSE, regs_t, cause);
    DEFINE_OFFSET(REGS_TVAL, regs_t, tval);
    DEFINE_OFFSET(REGS_PPC, regs_t, ppc);
    DEFINE_OFFSET(REGS_PSP, regs_t, psp);
    DEFINE_OFFSET(REGS_PA0, regs_t, pa0);
    DEFINE_OFFSET(REGS_PA1, regs_t, pa1);

    DEFINE_OFFSET(PROC_REGS, proc_t, regs);
    DEFINE_OFFSET(PROC_PID, proc_t, pid);
    DEFINE_OFFSET(PROC_STATE, proc_t, state);
    DEFINE_OFFSET(PROC_CAP_TABLE, proc_t, cap_table);
    DEFINE_OFFSET(PROC_CLIENT, proc_t, client);

    DEFINE_OFFSET(CAP_NODE_PREV, cap_node_t, prev);
    DEFINE_OFFSET(CAP_NODE_NEXT, cap_node_t, next);
    DEFINE_OFFSET(CAP_NODE_CAP, cap_node_t, cap);

    DEFINE_OFFSET(CAP_WORD0, cap_t, word0);
    DEFINE_OFFSET(CAP_WORD1, cap_t, word1);

    DEFINE_ENUM(NUM_OF_SYSNR);
}
