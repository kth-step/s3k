/* See LICENSE file for copyright and license details. */
#include "proc.h"

#include "csr.h"

static inline uint64_t get_byte(uint64_t word, uint64_t i)
{
	return (word >> (i * 8)) & 0xFFull;
}

static inline void set_byte(uint64_t *word, uint64_t val, uint64_t i)
{
	uint64_t mask = 0xFFull << (i * 8);
	*word = (*word & ~mask) | (val << (i * 8));
}

struct proc processes[NPROC];

void proc_loadpmp(const struct proc *proc)
{
	csrw_pmpcfg0(proc->pmpcfg0);
	csrw_pmpaddr0(proc->pmpaddr[0]);
	csrw_pmpaddr1(proc->pmpaddr[1]);
	csrw_pmpaddr2(proc->pmpaddr[2]);
	csrw_pmpaddr3(proc->pmpaddr[3]);
	csrw_pmpaddr4(proc->pmpaddr[4]);
	csrw_pmpaddr5(proc->pmpaddr[5]);
	csrw_pmpaddr6(proc->pmpaddr[6]);
	csrw_pmpaddr7(proc->pmpaddr[7]);
}
