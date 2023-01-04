/* See LICENSE file for copyright and license details. */
#ifndef __PROC_H__
#define __PROC_H__
#include <stdint.h>

#include "cap_node.h"
#include "lock.h"

#define REG_COUNT (sizeof(struct regs) / sizeof(uint64_t))

struct regs {
	/* Regular registers */
	uint64_t pc, ra, sp, gp, tp;
	uint64_t t0, t1, t2;
	uint64_t s0, s1;
	uint64_t a0, a1, a2, a3, a4, a5, a6, a7;
	uint64_t s2, s3, s4, s5, s6, s7, s8, s9, s10, s11;
	uint64_t t3, t4, t5, t6;
	/* Trap registers */
	uint64_t cause, tval, epc, tvec;
	/* PMP control register */
	uint64_t pmp;
};

struct proc {
	struct regs regs;
	uint64_t pid;
	uint64_t state;
	uint64_t pmpcfg0;
	uint64_t pmpaddr[8];
	struct cap_node caps[NCAP] __attribute__((aligned(sizeof(struct cap_node))));
} __attribute__((aligned(2048)));

_Static_assert(sizeof(struct proc) <= 4096, "PCB is too large");

extern struct proc processes[NPROC];
void proc_loadpmp(const struct proc *proc);
#endif /* __PROC_H__ */
