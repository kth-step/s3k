#include "cap_table.h"
#include "cap_types.h"
#include "error.h"
#include "kernel.h"

err_t cap_pmp_load(cte_t pmp, pmp_slot_t slot)
{
	proc_t *proc = proc_get(cte_pid(pmp));
	cap_t pmp_cap = cte_cap(pmp);

	if (!pmp_cap.type)
		return ERR_EMPTY;
	if (pmp_cap.type != CAPTY_PMP || pmp_cap.pmp.used)
		return ERR_INVALID_PMP;
	if (!proc_pmp_avail(proc, slot))
		return ERR_DST_OCCUPIED;
	proc_pmp_load(proc, slot, pmp_cap.pmp.rwx, pmp_cap.pmp.addr);
	pmp_cap.pmp.slot = slot;
	pmp_cap.pmp.used = 1;
	cte_set_cap(pmp, pmp_cap);
	return SUCCESS;
}

err_t cap_pmp_unload(cte_t pmp)
{
	proc_t *proc = proc_get(cte_pid(pmp));
	cap_t pmp_cap = cte_cap(pmp);

	if (!pmp_cap.type)
		return ERR_EMPTY;
	if (pmp_cap.type != CAPTY_PMP || !pmp_cap.pmp.used)
		return ERR_INVALID_PMP;
	proc_pmp_unload(proc, pmp_cap.pmp.slot);
	pmp_cap.pmp.slot = 0;
	pmp_cap.pmp.used = 0;
	cte_set_cap(pmp, pmp_cap);
	return SUCCESS;
}
