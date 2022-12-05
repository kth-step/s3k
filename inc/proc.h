// See LICENSE file for copyright and license details.
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "cap_node.h"
#include "consts.h"

#define N_REGISTERS (sizeof(regs_t) / sizeof(uint64_t))

typedef struct regs regs_t;
typedef struct proc proc_t;

struct regs {
        /* Standard registers */
        uint64_t pc;
        uint64_t ra, sp, gp, tp;
        uint64_t t0, t1, t2;
        uint64_t s0, s1;
        uint64_t a0, a1, a2, a3, a4, a5, a6, a7;
        uint64_t s2, s3, s4, s5, s6, s7, s8, s9, s10, s11;
        uint64_t t3, t4, t5, t6;
        /* pmp capabilities */
        uint64_t pmp;
        /* timer */
        uint64_t timeout;
        /* Destination for received capabilities */
        uint64_t dest_cidx;
        /* Exception handling registers */
        uint64_t tpc, tsp, cause, tval;
        uint64_t ppc, psp, pa0, pa1;
};

struct proc {
        regs_t regs;
        uint64_t pid;
        uint64_t state;
        uint64_t dest_cidx;
        cap_node_t* cap_table;
        proc_t* client;
};

extern proc_t processes[N_PROC];
register proc_t* current __asm__("tp");

void proc_init(uint64_t init_payload);
void proc_load_pmp(proc_t* proc);

static inline cap_node_t* proc_get_cap_node(proc_t* proc, uint64_t cid);
static inline cap_t proc_get_cap(proc_t* proc, uint64_t cid);
static inline uint64_t proc_get_reg(proc_t* proc, uint64_t regi);
static inline uint64_t proc_set_reg(proc_t* proc, uint64_t regi, uint64_t regv);

cap_node_t* proc_get_cap_node(proc_t* proc, uint64_t cid)
{
        return &proc->cap_table[cid % N_CAPS];
}

cap_t proc_get_cap(proc_t* proc, uint64_t cid)
{
        return cap_node_get_cap(proc_get_cap_node(proc, cid));
}

uint64_t proc_get_reg(proc_t* proc, uint64_t reg)
{
        kassert(reg < N_REGISTERS);
        uint64_t* regs = (uint64_t*)&proc->regs;
        return regs[reg];
}

uint64_t proc_set_reg(proc_t* proc, uint64_t reg, uint64_t val)
{
        kassert(reg < N_REGISTERS);
        uint64_t* regs = (uint64_t*)&proc->regs;
        uint64_t tmp = regs[reg];
        regs[reg] = val;
        return tmp;
}
