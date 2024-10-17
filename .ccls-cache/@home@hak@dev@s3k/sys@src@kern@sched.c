/* See LICENSE file for copyright and license details. */

#include "drivers/time.h"
#include "kern/csr.h"
#include "kern/kassert.h"
#include "kern/kprintf.h"
#include "kern/proc.h"
#include "kern/sched.h"
#include "kern/trap.h"
#include "kern/types.h"
#include "kern/wfi.h"
#include "smp/semaphore.h"

typedef struct {
	// Owner of time slot.
	int pid;
	// Remaining length of corresponding slice.
	int len;
} SlotInfo;

static SlotInfo slots[S3K_SLOT_CNT];

void SchedInit(void)
{
	sched_update(0, 0, 0, S3K_SLOT_CNT);
}

void SchedUpdate(Word pid, Word hart, Word bgn, Word end)
{
	for (int i = bgn; i < end; i++)
		slots[i] = (SlotInfo){.pid = pid, .len = end - i};
}

void SchedDelete(Word hart, Word bgn, Word end)
{
	for (int i = bgn; i < end; ++i)
		slots[i].len = 0;
}

static SlotInfo slot_info_get(Word hart, Word slot)
{
	return slots[slot % S3K_SLOT_CNT];
}

static Proc *SchedFetch(Word hart, Word slot)
{
	// Get time slot information
	SlotInfo si = slot_info_get(hart, slot);
	Proc *proc = ProcGet(si.pid);

	// If length = 0, then slice is deleted.
	if (si.len == 0)
		return NULL;

	// Try to acquire the process.
	if (!ProcAcquire(proc))
		return NULL;

	// Get the process.
	proc->timeout = (slot + si.len) * S3K_SLOT_LEN;
	return proc;
}

Proc *SchedNext(void)
{
	// Hart ID
	Word hart = csrr(mhartid);
	// Process to schedule
	Proc *proc;

	timeout_set(hart, (uint64_t)-1);

	do {
		uintptr_t slot = time_get() / S3K_SLOT_LEN;
		while (time_get() < slot * S3K_SLOT_LEN)
			;
		// Try schedule process
		proc = sched_fetch(hart, slot);
	} while (!proc);
	timeout_set(hart, proc->timeout);
	return proc;
}
