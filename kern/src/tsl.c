#include "tsl.h"

#include "macro.h"
#include "preempt.h"
#include "proc.h"
#include "sched.h"

/**
 * Table of time slice capabilities.
 */
static tsl_t tsl_table[TSL_TABLE_SIZE];

/**
 * Initializes the time slice capabilities.
 */
void tsl_init()
{
	// Create an initial time slice capability for each hardware thread.
	for (int i = 0; i < NUM_HARTS; ++i) {
		tsl_table[i * MAX_TIME_FUEL] = (tsl_t){
			.owner = 1,
			.base = 0,
			.hart = i,
			.cfree = MAX_TIME_FUEL,
			.csize = MAX_TIME_FUEL,
			.free = MAX_TIME_SLOT,
			.size = MAX_TIME_SLOT,
			.enabled = (i == 0) // Enable the first hart by default.,
		};
	}
}

/**
 * Validates the arguments for accessing a time slice capability.
 */
bool tsl_valid_access(pid_t owner, index_t i)
{
	return i < ARRAY_SIZE(tsl_table) && tsl_table[i].owner == owner;
}

/**
 * Checks if a time slice capability can be derived from another.
 */
static bool _derivable(tsl_t parent, fuel_t csize, time_slot_t size)
{
	return parent.cfree > csize && size <= parent.free && csize > 0 && size > 0;
}

/**
 * Transfers a time slice capability from one process to another.
 */
int tsl_transfer(pid_t owner, index_t i, pid_t new_owner)
{
	if (UNLIKELY(!tsl_valid_access(owner, i))) {
		return ERR_INVALID_ACCESS;
	}

	// Update the owner of the capability.
	tsl_table[i].owner = new_owner;

	// Update the scheduler if the capability is enabled.
	if (tsl_table[i].free > 0) {
		sched_set_pid(tsl_table[i].hart, tsl_table[i].enabled ? new_owner : INVALID_PID, tsl_table[i].base);
	}

	return ERR_SUCCESS;
}

int tsl_introspect(pid_t owner, index_t i, fuel_t offset, tsl_t *cap)
{
	if (UNLIKELY(!tsl_valid_access(owner, i))) {
		return ERR_INVALID_ACCESS;
	}

	if (offset >= tsl_table[i].csize) {
		return ERR_INVALID_ARGUMENT;
	}

	*cap = tsl_table[i + offset];
	return ERR_SUCCESS;
}

/**
 * Derives a new time slice capability from an existing one.
 */
int tsl_derive(pid_t owner, index_t i, pid_t target, fuel_t csize, bool enable, time_slot_t size)
{
	if (UNLIKELY(!tsl_valid_access(owner, i))) {
		return ERR_INVALID_ACCESS;
	}

	if (UNLIKELY(!_derivable(tsl_table[i], csize, size))) {
		return ERR_INVALID_ARGUMENT;
	}

	// Update the parent capability by reducing its cfree and adjusting its allocation.
	tsl_table[i].cfree -= csize;
	tsl_table[i].free -= size;

	// Calculate the index for the derived capability.
	index_t j = i + tsl_table[i].cfree;

	// Calculate the start of the new time slice capability
	time_slot_t base = tsl_table[i].base + tsl_table[i].free;

	// Create the new child capability with the specified parameters.
	tsl_table[j] = (tsl_t){
		.owner = target,
		.cfree = csize,
		.csize = csize,
		.hart = tsl_table[i].hart,
		.enabled = enable,
		.base = base,
		.size = size,
		.free = size,
	};

	// Update the scheduler with the new capability.
	pid_t sched_pid = enable ? target : INVALID_PID;
	sched_split(tsl_table[i].hart, sched_pid, tsl_table[i].base, base, base + size);

	// Return the index of the new capability.
	return j;
}

/**
 * Revokes the children of a time slice capability.
 */
int tsl_revoke(pid_t owner, index_t i)
{
	if (UNLIKELY(!tsl_valid_access(owner, i))) {
		return ERR_INVALID_ACCESS;
	}

	// Reclaim cfree and invalidate children.
	while (tsl_table[i].cfree < tsl_table[i].csize) {
		index_t j = i + tsl_table[i].cfree;

		// Reclaim cfree and allocation.
		tsl_table[i].cfree += tsl_table[j].cfree;
		tsl_table[i].free += tsl_table[j].free;

		// Invalidate the child capability.
		tsl_table[j].owner = INVALID_PID;

		if (UNLIKELY(preempt()))
			break;
	}

	// Reclaim allocated time slots in the scheduler.
	pid_t pid = tsl_table[i].enabled ? owner : INVALID_PID;
	sched_reclaim(tsl_table[i].hart, pid, tsl_table[i].base, tsl_table[i].base + tsl_table[i].free);

	// Return the remaining cfree to be revoked.
	// Is 0 if all children are revoked.
	return tsl_table[i].csize - tsl_table[i].cfree;
}

/**
 * Deletes a time slice capability
 */
int tsl_delete(pid_t owner, index_t i)
{
	if (UNLIKELY(!tsl_valid_access(owner, i))) {
		return ERR_INVALID_ACCESS;
	}

	// Invalidates the capability.
	tsl_table[i].owner = INVALID_PID;

	// Deletes the minor frame in the scheduler.
	if (tsl_table[i].free > 0) {
		sched_set_pid(tsl_table[i].hart, INVALID_PID, tsl_table[i].base);
	}

	return ERR_SUCCESS;
}

/**
 * Enables or disables a time slice capability.
 */
int tsl_set(pid_t owner, index_t i, bool enable)
{
	if (UNLIKELY(!tsl_valid_access(owner, i))) {
		return ERR_INVALID_ACCESS;
	}

	// Enable or disable the minor frame in the scheduler.
	if (tsl_table[i].free > 0) {
		pid_t sched_pid = enable ? owner : INVALID_PID;
		sched_set_pid(tsl_table[i].hart, sched_pid, tsl_table[i].base);
	}
	// Make the time slice capability enabled or disabled.
	tsl_table[i].enabled = enable;

	return ERR_SUCCESS;
}
