#include "cap_monitor.h"

#include "cap_ops.h"
#include "cap_pmp.h"
#include "proc.h"

static err_t check_monitor(cte_t mon, pid_t pid, bool check_suspended)
{
	cap_t mon_cap = cte_cap(mon);
	if (mon_cap.type != CAPTY_MONITOR || mon_cap.mon.mrk > pid
	    || pid >= mon_cap.mon.end)
		return ERR_INVALID_MONITOR;
	if (check_suspended && !proc_is_suspended(proc_get(pid)))
		return ERR_INVALID_STATE;
	return SUCCESS;
}

static err_t check_monitor_move(cte_t mon, cte_t src, cte_t dst)
{
	uint64_t mon_pid = cte_pid(mon);
	uint64_t src_pid = cte_pid(src);
	uint64_t dst_pid = cte_pid(dst);
	err_t err;
	if (mon_pid != src_pid && (err = check_monitor(mon, src_pid, true)))
		return err;
	if (mon_pid != dst_pid && (err = check_monitor(mon, dst_pid, true)))
		return err;
	return SUCCESS;
}

err_t cap_monitor_suspend(cte_t mon, pid_t pid)
{
	err_t err = check_monitor(mon, pid, false);
	if (!err)
		proc_suspend(proc_get(pid));
	return err;
}

err_t cap_monitor_resume(cte_t mon, pid_t pid)
{
	err_t err = check_monitor(mon, pid, false);
	if (!err)
		proc_resume(proc_get(pid));
	return err;
}

err_t cap_monitor_state_get(cte_t mon, pid_t pid, proc_state_t *state)
{
	err_t err = check_monitor(mon, pid, false);
	if (!err) {
		proc_t *proc = proc_get(pid);
		*state = proc->state;
	}
	return err;
}

err_t cap_monitor_yield(cte_t mon, pid_t pid, proc_t **next)
{
	err_t err = check_monitor(mon, pid, false);
	if (!err) {
		proc_t *proc = proc_get(pid);
		proc_t *curr = proc_get(cte_pid(mon));
		if (proc_acquire(proc)) {
			curr->regs[REG_T0] = SUCCESS;
			*next = proc;
			return YIELD;
		}
		curr->regs[REG_T0] = ERR_INVALID_STATE;
	}
	return err;
}

err_t cap_monitor_reg_read(cte_t mon, pid_t pid, reg_t reg, uint64_t *val)
{
	err_t err = check_monitor(mon, pid, true);
	if (!err)
		*val = proc_get(pid)->regs[reg];
	return err;
}

err_t cap_monitor_reg_write(cte_t mon, pid_t pid, reg_t reg, uint64_t val)
{
	err_t err = check_monitor(mon, pid, true);
	if (!err)
		proc_get(pid)->regs[reg] = val;
	return err;
}

err_t cap_monitor_cap_read(cte_t mon, cte_t src, cap_t *cap)
{
	err_t err = check_monitor(mon, cte_pid(src), true);
	if (!err)
		err = cap_read(src, cap);
	return err;
}

err_t cap_monitor_cap_move(cte_t mon, cte_t src, cte_t dst)
{
	err_t err = check_monitor_move(mon, src, dst);
	cap_t cap;
	if (!err)
		err = cap_move(src, dst, &cap);
	return err;
}

err_t cap_monitor_pmp_load(cte_t mon, cte_t pmp, pmp_slot_t slot)
{
	err_t err = check_monitor(mon, cte_pid(pmp), true);
	if (!err)
		err = cap_pmp_load(pmp, slot);
	return err;
}

err_t cap_monitor_pmp_unload(cte_t mon, cte_t pmp)
{
	err_t err = check_monitor(mon, cte_pid(pmp), true);
	if (!err)
		err = cap_pmp_unload(pmp);
	return err;
}
