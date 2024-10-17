/* See LICENSE file for copyright and license details. */
#include "drivers/time.h"
#include "kern/cap.h"
#include "kern/csr.h"
#include "kern/kassert.h"
#include "kern/proc.h"

extern Word _payload[];
static Proc procs[S3K_PROC_CNT];

void ProcInit(void)
{
	for (uint64_t i = 0; i < S3K_PROC_CNT; i++) {
		procs[i].pid = i;
		procs[i].state = PSF_SUSPENDED;
	}
	procs[0].state = 0;
	procs[0].regs.pc = (reg_t)_payload;
	// KASSERT(cap_pmp_load(ctable_get(0, 0), 0) == SUCCESS);
}

Proc *ProcGet(pid_t pid)
{
	KASSERT(pid < S3K_PROC_CNT);
	KASSERT(procs[pid].pid == pid);
	return &procs[pid];
}

ProcState ProcGetState(Proc *proc)
{
	Proc state = proc->state;
	if ((state == PSF_BLOCKED) && time_get() >= proc->timeout) {
		// TODO: Unblock state.
		return 0;
	}
	return state;
}

bool ProcAcquire(Proc *proc)
{
	ProcState expected = ProcGetState(proc);
	proc_state_t desired = PSF_BUSY;

	if (expected & (PSF_BUSY | PSF_SUSPENDED))
		return false;

	if (time_get() < proc->timeout)
		return false;
#ifdef SMP
	return __atomic_compare_exchange(&proc->state, &expected, &desired, false, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED);
#else
	proc->state = desired;
	return true;
#endif
}

void ProcRelease(Proc *proc)
{
	KASSERT(proc->state & PSF_BUSY);
#ifdef SMP
	__atomic_fetch_xor(&proc->state, PSF_BUSY, __ATOMIC_RELEASE);
#else
	proc->state &= ~PSF_BUSY;
#endif
}

void ProcSuspend(Proc *proc)
{
	ProcState prev = __atomic_fetch_or(&proc->state, PSF_SUSPENDED, __ATOMIC_RELAXED);
	if (prev & PSF_BLOCKED) {
		proc->state = PSF_SUSPENDED;
		proc->regs.t0 = ERR_SUSPENDED;
	}
}

void ProcResume(Proc *proc)
{
	if (proc->state == PSF_SUSPENDED)
		proc->timeout = 0;
	__atomic_fetch_and(&proc->state, ~PSF_SUSPENDED, __ATOMIC_RELAXED);
}

void ProcIpcWait(Proc *proc, Word chan)
{
	KASSERT(proc->state == PSF_BUSY);
	proc->state = PSF_BLOCKED | ((uint64_t)chan << 48) | PSF_BUSY;
}

bool ProcIpcAcquire(Proc *proc, chan_t chan)
{
	ProcState expected = PSF_BLOCKED | ((uint64_t)chan << 48);
	ProcState desired = PSF_BUSY;

	if (proc->state != expected)
		return false;
	if (time_get() >= proc->timeout)
		return false;
#ifdef SMP
	return __atomic_compare_exchange_n(&proc->state, &expected, desired, false, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED);
#else
	proc->state = desired;
	return true;
#endif
}

bool ProcIsSuspended(proc_t *proc)
{
	return proc->state == PSF_SUSPENDED;
}

bool proc_pmp_avail(proc_t *proc, pmp_slot_t slot)
{
	return proc->pmp.cfg[slot] == 0;
}

void proc_pmp_load(proc_t *proc, pmp_slot_t slot, pmp_slot_t rwx, napot_t addr)
{
	proc->pmp.cfg[slot] = (uint8_t)(rwx | 0x18);
	proc->pmp.addr[slot] = addr;
}

void proc_pmp_unload(proc_t *proc, pmp_slot_t slot)
{
	proc->pmp.cfg[slot] = 0;
}
