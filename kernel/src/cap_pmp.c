#include "cap_table.h"
#include "cap_types.h"
#include "error.h"
#include "kernel.h"

err_t cap_pmp_load(cte_t pmp, pmp_slot_t slot)
{
	proc_t *proc = proc_get(cte_pid(pmp));
	cap_t cap = cte_cap(pmp);
	if (cap.type == CAPTY_NONE)
		return ERR_EMPTY;
	if (cap.type != CAPTY_PMP || cap.pmp.used)
		return ERR_INVALID_PMP;
	if (!proc_pmp_avail(proc, slot))
		return ERR_DST_OCCUPIED;

	proc_pmp_load(proc, slot, cap.pmp.rwx, cap.pmp.addr);
	cap.pmp.slot = slot;
	cap.pmp.used = 1;
	cte_set_cap(pmp, cap);
	return SUCCESS;
}

err_t cap_pmp_unload(cte_t pmp)
{
	proc_t *proc = proc_get(cte_pid(pmp));
	cap_t cap = cte_cap(pmp);

	if (cap.type == CAPTY_NONE)
		return ERR_EMPTY;
	if (cap.type != CAPTY_PMP || !cap.pmp.used)
		return ERR_INVALID_PMP;

	proc_pmp_unload(proc, cap.pmp.slot);
	cap.pmp.slot = 0;
	cap.pmp.used = 0;
	cte_set_cap(pmp, cap);
	return SUCCESS;
}
