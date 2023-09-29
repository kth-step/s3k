/* See LICENSE file for copyright and license details. */
#include "proc.h"

#include "cap_pmp.h"
#include "csr.h"
#include "drivers/timer.h"
#include "kassert.h"

static proc_t _processes[S3K_PROC_CNT];
extern unsigned char _payload[];

void proc_init(void)
{
	for (uint64_t i = 0; i < S3K_PROC_CNT; i++) {
		_processes[i].pid = i;
		_processes[i].state = PSF_SUSPENDED;
		_processes[i].instrument_wcet = 0;
	}
	_processes[0].state = 0;
	_processes[0].tf.pc = (uint64_t)_payload;
	KASSERT(cap_pmp_load(ctable_get(0, 0), 0) == SUCCESS);
}

proc_t *proc_get(uint64_t pid)
{
	KASSERT(pid < S3K_PROC_CNT);
	KASSERT(_processes[pid].pid == pid);
	return &_processes[pid];
}

bool proc_acquire(proc_t *proc)
{
	// Set the busy flag if expected state
	uint64_t expected = proc->state;
	uint64_t desired = PSF_BUSY;

	// If state == 0, then process is ready.
	bool is_ready = (expected == 0);
	// If state is blocked, then process logically ready on timeout.
	bool is_timeout
	    = ((expected & PSF_BLOCKED) && time_get() >= proc->timeout);

	if (!is_ready && !is_timeout)
		return false;

	return __atomic_compare_exchange(&proc->state, &expected, &desired,
					 false /* not weak */,
					 __ATOMIC_ACQUIRE /* succ */,
					 __ATOMIC_RELAXED /* fail */);
}

void proc_release(proc_t *proc)
{
	// Unset all flags except suspend flag.
	__atomic_fetch_and(&proc->state, (uint64_t)~PSF_BUSY, __ATOMIC_RELEASE);
}

void proc_suspend(proc_t *proc)
{
	// Set the suspend flag
	uint64_t prev_state
	    = __atomic_fetch_or(&proc->state, PSF_SUSPENDED, __ATOMIC_ACQUIRE);

	// If the process was waiting, we also unset the waiting flag.
	if ((prev_state & 0xFF) == PSF_BLOCKED) {
		proc->tf.t0 = ERR_SUSPENDED;
		proc->state = PSF_SUSPENDED;
		__atomic_thread_fence(__ATOMIC_ACQUIRE);
	}
}

void proc_resume(proc_t *proc)
{
	// Unset the suspend flag
	__atomic_fetch_and(&proc->state, (uint64_t)~PSF_SUSPENDED,
			   __ATOMIC_RELEASE);
}

void proc_ipc_wait(proc_t *proc, uint64_t channel)
{
	KASSERT(proc->state == PSF_BUSY);
	proc->state = PSF_BLOCKED | (channel << 16);
}

bool proc_ipc_acquire(proc_t *proc, uint64_t channel)
{
	if (proc->timeout <= time_get())
		return false;
	uint64_t expected = PSF_BLOCKED | (channel << 16);
	uint64_t desired = PSF_BUSY;
	return __atomic_compare_exchange(&proc->state, &expected, &desired,
					 false, __ATOMIC_ACQUIRE,
					 __ATOMIC_RELAXED);
}

bool proc_is_suspended(proc_t *proc)
{
	return proc->state == PSF_SUSPENDED;
}

bool proc_pmp_avail(proc_t *proc, uint64_t slot)
{
	return proc->pmpcfg[slot] == 0;
}

void proc_pmp_load(proc_t *proc, uint64_t slot, uint64_t rwx, uint64_t addr)
{
	proc->pmpcfg[slot] = (uint8_t)(rwx | 0x18);
	proc->pmpaddr[slot] = addr;
}

void proc_pmp_unload(proc_t *proc, uint64_t slot)
{
	proc->pmpcfg[slot] = 0;
}

void proc_pmp_sync(proc_t *proc)
{
	csrw_pmpcfg0(*((uint64_t *)proc->pmpcfg));
	csrw_pmpaddr0(proc->pmpaddr[0]);
	csrw_pmpaddr1(proc->pmpaddr[1]);
	csrw_pmpaddr2(proc->pmpaddr[2]);
	csrw_pmpaddr3(proc->pmpaddr[3]);
	csrw_pmpaddr4(proc->pmpaddr[4]);
	csrw_pmpaddr5(proc->pmpaddr[5]);
	csrw_pmpaddr6(proc->pmpaddr[6]);
	csrw_pmpaddr7(proc->pmpaddr[7]);
}