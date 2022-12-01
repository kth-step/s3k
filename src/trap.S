// See LICENSE file for copyright and license details.
#include "platform.h"
#include "macros.S"
#include "offsets.g.h"

#define MCAUSE_U_ECALL 8
#define MSTATUS_IE 8

.globl trap_entry
.globl trap_resume_proc
.globl hang

.section .text.trap_entry
.balign 16
trap_entry:
        /* Save a0, load pcb */ 
        csrrw   tp,mscratch,tp

        /* if a0 == 0, then M-mode preemption */
        beqz    tp,_m_trap_timer   

        sd      ra,(PROC_REGS + REGS_RA)(tp)
        sd      sp,(PROC_REGS + REGS_SP)(tp)
        sd      gp,(PROC_REGS + REGS_GP)(tp)
        /*sd      tp,(PROC_REGS + REGS_TP)(tp)*/
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

        /* Load kernel gp and sp */
        load_gp
        load_sp

        /* Save tp and pc*/
        csrrw   t0,mscratch,zero
        csrr    t1,mepc
        sd      t0,(PROC_REGS + REGS_TP)(tp)
        sd      t1,(PROC_REGS + REGS_PC)(tp)

        /* mcause < 0, must be timer interrupt so yield */
        csrr    t0,mcause
        bltz    t0,_trap_timer
        
        /* if mcause == User ecall, it is a system call */ 
        addi    t0,t0,-MCAUSE_U_ECALL
        bnez    t0,_exception
        li      t0,NUM_OF_SYSNR
        bgeu    a0,t0,_exception

        la      ra,_syscall_table
        addi    t1,t1,4
        sd      t1,(PROC_REGS + REGS_PC)(tp)
        slli    a0,a0,2
        add     ra,ra,a0
        mv      a0,tp
        j       _jump

_exception:
        la      ra,exception_handler
        csrr    a1,mcause
        csrr    a2,mtval
        csrr    a3,mepc
        mv      a0,tp
        j       _jump

_m_trap_timer:
        csrrw   tp,mscratch,zero
        csrr    t0,mcause
        bgez    t0,hang
        load_sp

_trap_timer:
        la      ra,sched_yield
        mv      a0,tp

_jump:  jalr    ra,0(ra)    

trap_resume_proc:
        mv      tp,a0
        /* Enable preemption */
        csrw    mstatus,MSTATUS_IE

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
        /*ld      tp,(PROC_REGS + REGS_TP)(tp)*/
        /*ld      gp,(PROC_REGS + REGS_GP)(tp)*/
        /*ld      sp,(PROC_REGS + REGS_SP)(tp)*/
        ld      ra,(PROC_REGS + REGS_RA)(tp)

        /* Disable preemption */
        csrw    mstatus,0
        /* Save pointer to proc_t */
        csrw    mscratch,tp
        /* Restore user sp, gp and a0 */
        ld      sp,(PROC_REGS + REGS_SP)(tp)
        ld      gp,(PROC_REGS + REGS_GP)(tp)
        ld      tp,(PROC_REGS + REGS_TP)(tp)
        /* Return to user mode */
        mret


_syscall_table:
.option push
.option norvc
        j       syscall_get_pid
        j       syscall_get_reg
        j       syscall_set_reg
        j       syscall_yield
        /* basic capabilities */
        j       syscall_read_cap
        j       syscall_move_cap
        j       syscall_delete_cap
        j       syscall_revoke_cap
        j       syscall_derive_cap
        /* superviser capabilities functions */
        j       syscall_sup_suspend
        j       syscall_sup_resume
        j       syscall_sup_get_state
        j       syscall_sup_get_reg
        j       syscall_sup_set_reg
        j       syscall_sup_read_cap
        j       syscall_sup_move_cap
.option pop

_dummy:
        ret

.section .text.hang
hang:   wfi
        j       hang
