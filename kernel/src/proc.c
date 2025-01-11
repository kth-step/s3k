/* See LICENSE file for copyright and license details. */
#include "proc.h"

#include "cap_pmp.h"
#include "csr.h"
#include "drivers/time.h"
#include "kassert.h"

static proc_t procs[S3K_PROC_CNT];
extern unsigned char _payload[];

void proc_init(void)
{
	for (pid_t i = 0; i < S3K_PROC_CNT; i++) {
		procs[i].pid = i;
		procs[i].state = PSF_SUSPENDED;
	}
	procs[0].state = 0;
	procs[0].regs[REG_PC] = (val_t)_payload;
	KASSERT(cap_pmp_load(ctable_get(0, 0), 0) == SUCCESS);
	KASSERT(cap_pmp_load(ctable_get(0, 1), 1) == SUCCESS);
}

proc_t *proc_get(pid_t pid)
{
	KASSERT(pid < S3K_PROC_CNT);
	KASSERT(procs[pid].pid == pid);
	return &procs[pid];
}

proc_state_t proc_get_state(proc_t *proc)
{
	proc_state_t state = proc->state;
	if ((state == PSF_BLOCKED) && time_get() >= proc->timeout)
		return 0;
	return state;
}

bool proc_acquire(proc_t *proc)
{
	proc_state_t expected = proc->state;
	proc_state_t desired = PSF_BUSY;

	if (expected & (PSF_BUSY | PSF_SUSPENDED))
		return false;

	if (time_get() < proc->timeout)
		return false;
#ifdef SMP
	return __atomic_compare_exchange(&proc->state, &expected, &desired,
					 false, __ATOMIC_ACQUIRE,
					 __ATOMIC_RELAXED);
#else
	proc->state = desired;
	return true;
#endif
}

void proc_release(proc_t *proc)
{
	KASSERT(proc->state & PSF_BUSY);
#ifdef SMP
	__atomic_fetch_xor(&proc->state, PSF_BUSY, __ATOMIC_RELEASE);
#else
	proc->state &= ~PSF_BUSY;
#endif
}

void proc_suspend(proc_t *proc)
{
	proc_state_t prev
	    = __atomic_fetch_or(&proc->state, PSF_SUSPENDED, __ATOMIC_RELAXED);
	if (prev & PSF_BLOCKED) {
		proc->state = PSF_SUSPENDED;
		proc->regs[REG_T0] = ERR_SUSPENDED;
	}
}

void proc_resume(proc_t *proc)
{
	if (proc->state == PSF_SUSPENDED)
		proc->timeout = 0;
	__atomic_fetch_and(&proc->state, ~PSF_SUSPENDED, __ATOMIC_RELAXED);
}

void proc_ipc_wait(proc_t *proc, chan_t chan)
{
	KASSERT(proc->state == PSF_BUSY);
	proc->state = PSF_BLOCKED | ((val_t)chan << 16) | PSF_BUSY;
}

bool proc_ipc_acquire(proc_t *proc, chan_t chan)
{
	proc_state_t expected = PSF_BLOCKED | ((val_t)chan << 16);
	proc_state_t desired = PSF_BUSY;

	if (proc->state != expected)
		return false;
	if (time_get() >= proc->timeout)
		return false;
#ifdef SMP
	return __atomic_compare_exchange_n(&proc->state, &expected, desired,
					   false, __ATOMIC_ACQUIRE,
					   __ATOMIC_RELAXED);
#else
	proc->state = desired;
	return true;
#endif
}

bool proc_is_suspended(proc_t *proc)
{
	return proc->state == PSF_SUSPENDED;
}

bool proc_pmp_avail(proc_t *proc, pmp_slot_t slot)
{
	return proc->pmpcfg[slot] == 0;
}

void proc_pmp_load(proc_t *proc, pmp_slot_t slot, pmp_slot_t rwx, napot_t addr)
{
	proc->pmpcfg[slot] = (uint8_t)(rwx | 0x18);
	proc->pmpaddr[slot] = addr;
}

void proc_pmp_unload(proc_t *proc, pmp_slot_t slot)
{
	proc->pmpcfg[slot] = 0;
}

void proc_pmp_sync(proc_t *proc)
{
	csrw(pmpaddr0, proc->pmpaddr[0]);
	csrw(pmpaddr1, proc->pmpaddr[1]);
	csrw(pmpaddr2, proc->pmpaddr[2]);
	csrw(pmpaddr3, proc->pmpaddr[3]);
	csrw(pmpaddr4, proc->pmpaddr[4]);
	csrw(pmpaddr5, proc->pmpaddr[5]);
	csrw(pmpaddr6, proc->pmpaddr[6]);
	csrw(pmpaddr7, proc->pmpaddr[7]);
	csrw(pmpcfg0, *(val_t *)proc->pmpcfg);
}
