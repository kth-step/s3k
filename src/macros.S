// See LICENSE file for copyright and license details.

#include "platform.h"
#include "offsets.g.h"

.macro load_gp
        .option push
        .option norelax
        lla     gp,__global_pointer$
        .option pop
.endm

.macro load_sp tmp
        lla     sp,stack
        csrr    \tmp,mhartid
        addi    \tmp,\tmp,-MIN_HARTID
        slli    \tmp,\tmp,LOG_STACK_SIZE
        sub     sp,sp,\tmp
.endm

.macro save_ctx tmp0 tmp1
        sd      ra,(PROC_REGS + REGS_RA)(tp)
        sd      sp,(PROC_REGS + REGS_SP)(tp)
        sd      gp,(PROC_REGS + REGS_GP)(tp)
        sd      t0,(PROC_REGS + REGS_T0)(tp)
        sd      t1,(PROC_REGS + REGS_T1)(tp)
        sd      t2,(PROC_REGS + REGS_T2)(tp)
        sd      s0,(PROC_REGS + REGS_S0)(tp)
        sd      s1,(PROC_REGS + REGS_S1)(tp)
        sd      a0,(PROC_REGS + REGS_A0)(tp)
        sd      a1,(PROC_REGS + REGS_A1)(tp)
        sd      a2,(PROC_REGS + REGS_A2)(tp)
        sd      a3,(PROC_REGS + REGS_A3)(tp)
        sd      a4,(PROC_REGS + REGS_A4)(tp)
        sd      a5,(PROC_REGS + REGS_A5)(tp)
        sd      a6,(PROC_REGS + REGS_A6)(tp)
        sd      a7,(PROC_REGS + REGS_A7)(tp)
        sd      s2,(PROC_REGS + REGS_S2)(tp)
        sd      s3,(PROC_REGS + REGS_S3)(tp)
        sd      s4,(PROC_REGS + REGS_S4)(tp)
        sd      s5,(PROC_REGS + REGS_S5)(tp)
        sd      s6,(PROC_REGS + REGS_S6)(tp)
        sd      s7,(PROC_REGS + REGS_S7)(tp)
        sd      s8,(PROC_REGS + REGS_S8)(tp)
        sd      s9,(PROC_REGS + REGS_S9)(tp)
        sd      s10,(PROC_REGS + REGS_S10)(tp)
        sd      s11,(PROC_REGS + REGS_S11)(tp)
        sd      t3,(PROC_REGS + REGS_T3)(tp)
        sd      t4,(PROC_REGS + REGS_T4)(tp)
        sd      t5,(PROC_REGS + REGS_T5)(tp)
        sd      t6,(PROC_REGS + REGS_T6)(tp)

        csrrw   \tmp0,mscratch,zero
        sd      \tmp0,(PROC_REGS + REGS_TP)(tp)
        csrr    \tmp1,mepc
        sd      \tmp1,(PROC_REGS + REGS_PC)(tp)
.endm

.macro load_ctx
        /* Restore pc */
        ld      t0,(PROC_REGS + REGS_PC)(tp)
        csrw    mepc,t0

        /* Restore all registers except tp, sp, gp, pc */
        ld      t6,(PROC_REGS + REGS_T6)(tp)
        ld      t5,(PROC_REGS + REGS_T5)(tp)
        ld      t4,(PROC_REGS + REGS_T4)(tp)
        ld      t3,(PROC_REGS + REGS_T3)(tp)
        ld      s11,(PROC_REGS + REGS_S11)(tp)
        ld      s10,(PROC_REGS + REGS_S10)(tp)
        ld      s9,(PROC_REGS + REGS_S9)(tp)
        ld      s8,(PROC_REGS + REGS_S8)(tp)
        ld      s7,(PROC_REGS + REGS_S7)(tp)
        ld      s6,(PROC_REGS + REGS_S6)(tp)
        ld      s5,(PROC_REGS + REGS_S5)(tp)
        ld      s4,(PROC_REGS + REGS_S4)(tp)
        ld      s3,(PROC_REGS + REGS_S3)(tp)
        ld      s2,(PROC_REGS + REGS_S2)(tp)
        ld      a7,(PROC_REGS + REGS_A7)(tp)
        ld      a6,(PROC_REGS + REGS_A6)(tp)
        ld      a5,(PROC_REGS + REGS_A5)(tp)
        ld      a4,(PROC_REGS + REGS_A4)(tp)
        ld      a3,(PROC_REGS + REGS_A3)(tp)
        ld      a2,(PROC_REGS + REGS_A2)(tp)
        ld      a1,(PROC_REGS + REGS_A1)(tp)
        ld      a0,(PROC_REGS + REGS_A0)(tp)
        ld      s1,(PROC_REGS + REGS_S1)(tp)
        ld      s0,(PROC_REGS + REGS_S0)(tp)
        ld      t2,(PROC_REGS + REGS_T2)(tp)
        ld      t1,(PROC_REGS + REGS_T1)(tp)
        ld      t0,(PROC_REGS + REGS_T0)(tp)
        ld      ra,(PROC_REGS + REGS_RA)(tp)

        /* Disable preemption */
        csrw    mstatus,0
        /* Save pointer to proc_t */
        csrw    mscratch,tp

        /* Restore user sp, gp and tp */
        ld      sp,(PROC_REGS + REGS_SP)(tp)
        ld      gp,(PROC_REGS + REGS_GP)(tp)
        ld      tp,(PROC_REGS + REGS_TP)(tp)
.endm
