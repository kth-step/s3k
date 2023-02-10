/* See LICENSE file for copyright and license details. */
#include "proc.h"

#include "cnode.h"
#include "csr.h"

struct proc processes[NPROC];

void proc_load_pmp(const struct proc *proc)
{
	uint64_t pmp = proc->regs.pmp;
	uint64_t pmpcfg;
	uint64_t pmpaddr[8];
	cnode_handle_t handle;
	union cap cap;
	for (int i = 0; i < 8; i++) {
		handle = cnode_get_handle(proc->pid, pmp & 0xFF);
		cap = cnode_get_cap(handle);
		if (cap.type == CAPTY_PMP) {
			pmpcfg |= cap.pmp.cfg << (i * 8);
			pmpaddr[i] = cap.pmp.addr;
		} else {
			pmpaddr[i] = 0;
		}
		pmp >>= 8;
	}
	csrw_pmpcfg0(pmpcfg);
	csrw_pmpaddr0(pmpaddr[0]);
	csrw_pmpaddr1(pmpaddr[1]);
	csrw_pmpaddr2(pmpaddr[2]);
	csrw_pmpaddr3(pmpaddr[3]);
	csrw_pmpaddr4(pmpaddr[4]);
	csrw_pmpaddr5(pmpaddr[5]);
	csrw_pmpaddr6(pmpaddr[6]);
	csrw_pmpaddr7(pmpaddr[7]);
}
