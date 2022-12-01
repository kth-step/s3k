// See LICENSE file for copyright and license details.

#include "platform.h"

.macro zero_state
        li      x1,0
        li      x2,0
        li      x3,0
        li      x4,0
        li      x5,0
        li      x6,0
        li      x7,0
        li      x8,0
        li      x9,0
        li      x10,0
        li      x11,0
        li      x12,0
        li      x13,0
        li      x14,0
        li      x15,0
        li      x16,0
        li      x17,0
        li      x18,0
        li      x19,0
        li      x20,0
        li      x21,0
        li      x22,0
        li      x23,0 
        li      x24,0
        li      x25,0
        li      x26,0
        li      x27,0
        li      x28,0
        li      x29,0
        li      x30,0
        li      x31,0
.endm

.macro load_gp
        .option push
        .option norelax
        lla     gp,__global_pointer$
        .option pop
.endm

.macro load_sp
        lla     sp,stack
        csrr    t1,mhartid
        addi    t1,t1,-MIN_HARTID
        slli    t1,t1,LOG_STACK_SIZE
        sub     sp,sp,t1
.endm
