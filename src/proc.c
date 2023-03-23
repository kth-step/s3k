/* See LICENSE file for copyright and license details. */
#include "proc.h"

#include "assert.h"
#include "cnode.h"
#include "csr.h"

static struct proc processes[NPROC];

void proc_init(uint64_t payload)
{
	for (int i = 1; i < NPROC; i++) {
		processes[i] = (struct proc){ .pid = i, .state = PS_SUSPENDED };
	}
	processes[0] = (struct proc){ .pid = 0, .state = PS_READY };
	processes[0].regs[REG_PC] = payload;
}

struct proc *proc_get(uint64_t pid)
{
	assert(pid < NPROC);
	return &processes[pid];
}

bool proc_acquire(struct proc *proc, uint64_t expected)
{
	assert(!(expected & PSF_BUSY));
	// Set the busy flag if expected state
	uint64_t desired = expected | PSF_BUSY;
	return __atomic_compare_exchange_n(&proc->state, &expected, desired,
					   false /* not weak */,
					   __ATOMIC_ACQUIRE /* succ */,
					   __ATOMIC_RELAXED /* fail */);
}

void proc_release(struct proc *proc)
{
	// Unset the busy flag
	__atomic_fetch_and(&proc->state, ~PSF_BUSY, __ATOMIC_RELEASE);
}

void proc_suspend(struct proc *proc)
{
	// Set the suspend flag
	__atomic_fetch_or(&proc->state, PSF_SUSPEND, __ATOMIC_ACQUIRE);
}

void proc_resume(struct proc *proc)
{
	// Unset the suspend flag
	__atomic_fetch_and(&proc->state, ~PSF_SUSPEND, __ATOMIC_RELEASE);
}

void proc_load_pmp(const struct proc *proc)
{
	uint64_t pmp = proc->regs[REG_PMP];
	uint64_t pmpaddr[8];
	uint64_t pmpcfg = 0;
	cnode_handle_t handle;
	union cap cap;
	for (int i = 0; i < 8; i++) {
		handle = cnode_get_handle(proc->pid, pmp & 0xFF);
		cap = cnode_get_cap(handle);
		if (cap.type == CAPTY_PMP) {
			pmpcfg |= (uint64_t)cap.pmp.cfg << (i * 8);
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
