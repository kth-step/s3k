// See LICENSE file for copyright and license details.
#include "platform.h"
#include "macros.S"
#include "offsets.h"

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

        save_ctx t1 t2

        /* Load kernel gp and sp */
        load_gp
        load_sp t1

        /* Save tp and pc*/

        /* mcause < 0, must be timer interrupt so yield */
        csrr    t1,mcause
        bltz    t1,_trap_timer
        
        /* if mcause == User ecall, it is a system call */ 
        addi    t1,t1,-MCAUSE_U_ECALL
        bnez    t1,_exception
        li      t1,NUM_OF_SYSNR
        bgeu    t0,t1,_exception

/* Incprement PC */
        addi    t2,t2,4
        sd      t2,(PROC_REGS + REGS_PC)(tp)

1:      auipc   ra,%pcrel_hi(_syscall_table)
        slli    t0,t0,2
        add     ra,ra,t0
        jalr    ra,%pcrel_lo(1b)
        sd      a0,(PROC_REGS + REGS_T0)(tp)
        j       trap_resume_proc

_exception:
        la      ra,exception_handler
        csrr    a0,mcause
        csrr    a1,mtval
        csrr    a2,mepc
        j       _jump

_m_trap_timer:
        csrrw   tp,mscratch,zero
        csrr    t0,mcause
        bgez    t0,hang
        load_sp t0

_trap_timer:
        la      ra,sched_yield

_jump:  jalr    ra,0(ra)    

trap_resume_proc:

        /* Enable preemption */
        csrw    mstatus,MSTATUS_IE

        /* Load context */
        /* This is mostly preemptable */
        load_ctx

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
