#include "mon.h"

#include "macro.h"
#include "preempt.h"
#include "proc.h"
#include "types.h"

/**
 * Table of monitor capabilities.
 */
static mon_t mon_table[MON_TABLE_SIZE];

/**
 * Initializes the monitor capabilities for each process.
 */
void mon_init(void)
{
	for (int i = 0; i < MAX_PID; ++i) {
		mon_table[i * MAX_MONITOR_FUEL] = (mon_t){
			.owner = 1,
			.pid = (i + 1),
			.cfree = MAX_MONITOR_FUEL,
			.csize = MAX_MONITOR_FUEL,
		};
	}
}

/**
 * Validates the arguments for accessing a monitor capability.
 */
bool mon_valid_access(pid_t owner, index_t i)
{
	return i < ARRAY_SIZE(mon_table) && mon_table[i].owner == owner;
}

/**
 * Transfers a monitor capability to a new owner.
 */
int mon_transfer(pid_t owner, index_t i, pid_t new_owner)
{
	if (!mon_valid_access(owner, i)) {
		return ERR_INVALID_ACCESS;
	}

	// Update the owner of the capability.
	mon_table[i].owner = new_owner;
	return ERR_SUCCESS;
}

/**
 * Retrieves a monitor capability.
 */
int mon_get(pid_t owner, index_t i, mon_t *cap_monitor)
{
	if (UNLIKELY(!mon_valid_access(owner, i))) {
		return ERR_INVALID_ACCESS;
	}

	// Copy the capability to the output parameter.
	*cap_monitor = mon_table[i];
	return ERR_SUCCESS;
}

/**
 * Retrieves the process ID associated with a monitor capability.
 */
pid_t mon_get_pid(pid_t owner, index_t i)
{
	if (UNLIKELY(!mon_valid_access(owner, i))) {
		return INVALID_PID;
	}
	return mon_table[i].pid;
}

/**
 * Derives a new monitor capability from an existing one.
 */
int mon_derive(pid_t owner, index_t i, pid_t target, fuel_t csize)
{
	if (UNLIKELY(!mon_valid_access(owner, i))) {
		return ERR_INVALID_ACCESS;
	}

	if (UNLIKELY(csize <= 0 || mon_table[i].cfree < csize)) {
		return ERR_INVALID_ARGUMENT;
	}

	// Update the parent capability.
	mon_table[i].cfree -= csize;

	// Calculate the index for the derived capability.
	index_t j = i + mon_table[i].cfree;

	// Create the new child capability.
	mon_table[j] = (mon_t){
		.owner = target,
		.cfree = csize,
		.csize = csize,
		.pid = mon_table[i].pid,
	};

	// Return the index of the new child capability.
	return j;
}

/**
 * Revokes a monitor capability and its children.
 */
int mon_revoke(pid_t owner, index_t i)
{
	if (UNLIKELY(!mon_valid_access(owner, i))) {
		return ERR_INVALID_ACCESS;
	}

	while (mon_table[i].cfree < mon_table[i].csize) {
		// Get the index of the next child capability.
		index_t j = i + mon_table[i].cfree;

		// Invalidate the child capability.
		mon_table[j].owner = INVALID_PID;

		// Reclaim the child's resources.
		mon_table[i].cfree += mon_table[j].cfree;

		// Check for preemption.
		if (UNLIKELY(preempt()))
			break;
	}

	// Return the number of unrevoked capabilities.
	return mon_table[i].csize - mon_table[i].cfree;
}

/**
 * Deletes a monitor capability.
 */
int mon_delete(pid_t owner, index_t i)
{
	if (UNLIKELY(!mon_valid_access(owner, i))) {
		return ERR_INVALID_ACCESS;
	}

	// Invalidate the monitor capability.
	mon_table[i].owner = INVALID_PID;

	return ERR_SUCCESS;
}

/**
 * Suspends a process associated with the monitor capability.
 */
int mon_suspend(pid_t owner, index_t i)
{
	if (UNLIKELY(!mon_valid_access(owner, i))) {
		return ERR_INVALID_ACCESS;
	}
	proc_suspend(mon_table[i].pid);
	return ERR_SUCCESS;
}

/**
 * Resumes a suspended process associated with the monitor capability.
 */
int mon_resume(pid_t owner, index_t i)
{
	if (UNLIKELY(!mon_valid_access(owner, i))) {
		return ERR_INVALID_ACCESS;
	}
	proc_resume(mon_table[i].pid);
	return ERR_SUCCESS;
}

/**
 * Yield the execution time to the process associated with the monitor capability.
 */
int mon_yield(pid_t owner, index_t i, proc_t **next)
{
	if (UNLIKELY(!mon_valid_access(owner, i))) {
		return ERR_INVALID_ACCESS;
	}
	if (UNLIKELY(!proc_acquire(mon_table[i].pid))) {
		return ERR_INVALID_STATE;
	}
	*next = proc_get(mon_table[i].pid);
	return ERR_SUCCESS;
}

/**
 * Gets a register value for the process associated with the monitor capability.
 */

int mon_reg_get(pid_t owner, index_t i, word_t reg, word_t *value)
{
	if (UNLIKELY(!mon_valid_access(owner, i))) {
		return ERR_INVALID_ACCESS;
	}

	if (reg >= 32) {
		return ERR_INVALID_ARGUMENT; // Invalid register index.
	}

	proc_t *proc = proc_get(mon_table[i].pid);
	word_t *reg_ptr = (word_t *)&proc->regs;
	*value = reg_ptr[reg];
	return ERR_SUCCESS;
}

/**
 * Sets a register value for the process associated with the monitor capability.
 */
int mon_reg_set(pid_t owner, index_t i, word_t reg, word_t value)
{
	if (UNLIKELY(!mon_valid_access(owner, i))) {
		return ERR_INVALID_ACCESS;
	}

	if (reg >= 32) {
		return ERR_INVALID_ARGUMENT; // Invalid register index.
	}

	proc_t *proc = proc_get(mon_table[i].pid);
	word_t *reg_ptr = (word_t *)&proc->regs;
	reg_ptr[reg] = value;
	return ERR_SUCCESS;
}

/**
 * Gets a virtual register value for the process associated with the monitor capability.
 */
int mon_vreg_get(pid_t owner, index_t i, vreg_t reg, word_t *value)
{
	if (UNLIKELY(!mon_valid_access(owner, i))) {
		return ERR_INVALID_ACCESS;
	}

	proc_t *proc = proc_get(mon_table[i].pid);

	switch (reg) {
	case VREG_TPC:
		*value = proc->trap.tpc;
		return ERR_SUCCESS;
	case VREG_TSP:
		*value = proc->trap.tsp;
		return ERR_SUCCESS;
	case VREG_ECAUSE:
		*value = proc->trap.ecause;
		return ERR_SUCCESS;
	case VREG_EVAL:
		*value = proc->trap.eval;
		return ERR_SUCCESS;
	case VREG_EPC:
		*value = proc->trap.epc;
		return ERR_SUCCESS;
	case VREG_ESP:
		*value = proc->trap.esp;
		return ERR_SUCCESS;
	default:
		*value = 0;
		return ERR_INVALID_ARGUMENT;
	}
}

/**
 * Sets a virtual register value for the process associated with the monitor capability.
 */
int mon_vreg_set(pid_t owner, index_t i, vreg_t reg, word_t value)
{
	if (UNLIKELY(!mon_valid_access(owner, i))) {
		return ERR_INVALID_ACCESS;
	}

	proc_t *proc = proc_get(mon_table[i].pid);

	switch (reg) {
	case VREG_TPC:
		proc->trap.tpc = value;
		return ERR_SUCCESS;
	case VREG_TSP:
		proc->trap.tsp = value;
		return ERR_SUCCESS;
	case VREG_ECAUSE:
		proc->trap.ecause = value;
		return ERR_SUCCESS;
	case VREG_EVAL:
		proc->trap.eval = value;
		return ERR_SUCCESS;
	case VREG_EPC:
		proc->trap.epc = value;
		return ERR_SUCCESS;
	case VREG_ESP:
		proc->trap.esp = value;
		return ERR_SUCCESS;
	default:
		return ERR_INVALID_ARGUMENT;
	}
}
