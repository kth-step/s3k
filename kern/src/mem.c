#include "mem.h"

#include "macro.h"
#include "pmp.h"
#include "preempt.h"
#include "proc.h"

/**
 * Table of memory capabilities.
 */
static mem_t mem_table[MEM_TABLE_SIZE];

/**
 * Initialize the memory capabilities.
 */
void mem_init(mem_t init_mems[])
{
	for (index_t i = 0; i < NUM_MEMORY_CAPS; ++i) {
		mem_table[i * MAX_MEMORY_FUEL] = (mem_t){
			.owner = 1,
			.base = init_mems[i].base,
			.size = init_mems[i].size,
			.rwx = init_mems[i].rwx,
			.cfree = MAX_MEMORY_FUEL,
			.csize = MAX_MEMORY_FUEL,
		};
	}
}

/**
 * Check if a memory access is valid.
 */
bool mem_valid_access(pid_t owner, index_t i)
{
	return (i < ARRAY_SIZE(mem_table)) && (mem_table[i].owner == owner);
}

/**
 * Check if the read-write-execute permissions are valid.
 */
static bool _valid_rwx(word_t rwx)
{
	return ((rwx & MEM_PERM_R) == MEM_PERM_R) || (rwx == MEM_PERM_NONE);
}

/**
 * Check if a memory capability can be derived.
 */
static bool _derivable(mem_t parent, fuel_t csize, word_t rwx, word_t base, word_t size)
{
	return (base + size > base) && (parent.cfree > csize) && (parent.base <= base)
	       && (base + size <= parent.base + parent.size) && ((parent.rwx & rwx) == rwx) && (csize > 0)
	       && _valid_rwx(rwx);
}

/**
 * Check if the PMP arguments are valid.
 */
static bool _valid_pmp_args(mem_t cap, word_t slot, word_t rwx, word_t addr)
{
	word_t base = pmp_napot_decode_base(addr);
	word_t size = pmp_napot_decode_size(addr);

	return (slot > 0) && (slot <= MAX_PMP_SLOT) && (cap.base <= base) && (base + size <= cap.base + cap.size)
	       && ((rwx & cap.rwx) == rwx) && _valid_rwx(rwx);
}

/**
 * Transfer a memory capability to a new owner.
 */
int mem_transfer(pid_t owner, index_t i, pid_t new_owner)
{
	if (UNLIKELY(!mem_valid_access(owner, i))) {
		return ERR_INVALID_ACCESS;
	}

	// If PMP config is set, clear it.
	if (mem_table[i].slot != 0) {
		proc_pmp_clear(owner, mem_table[i].slot - 1);
		mem_table[i].slot = 0; // Clear the PMP slot.
	}

	// Set the new owner.
	mem_table[i].owner = new_owner;

	return ERR_SUCCESS;
}

/**
 * Get a memory capability.
 */
int mem_get(pid_t owner, index_t i, mem_t *cap)
{
	if (UNLIKELY(!mem_valid_access(owner, i))) {
		return ERR_INVALID_ACCESS;
	}

	*cap = mem_table[i];
	return ERR_SUCCESS;
}

int mem_introspect(pid_t owner, index_t i, fuel_t offset, mem_t *cap)
{
	if (UNLIKELY(!mem_valid_access(owner, i))) {
		return ERR_INVALID_ACCESS;
	}

	if (offset >= mem_table[i].csize) {
		return ERR_INVALID_ARGUMENT;
	}

	*cap = mem_table[i + offset];
	return ERR_SUCCESS;
}

/**
 * Derive a memory capability.
 */
int mem_derive(pid_t owner, index_t i, pid_t target, fuel_t cfree, mem_perm_t rwx, mem_addr_t base, mem_addr_t size)
{
	if (UNLIKELY(!mem_valid_access(owner, i))) {
		return ERR_INVALID_ACCESS;
	}

	// Check if the memory capability can be derived.
	if (UNLIKELY(!_derivable(mem_table[i], cfree, rwx, base, size))) {
		return ERR_INVALID_ARGUMENT;
	}

	// Update the current memory capability.
	mem_table[i].cfree -= cfree;

	// Calculate the index of the new memory capability.
	word_t j = i + mem_table[i].cfree;

	// Create the new memory capability.
	mem_table[j] = (mem_t){
		.owner = target,
		.cfree = cfree,
		.csize = cfree,
		.slot = 0,
		.rwx = rwx,
		.base = base,
		.size = size,
	};

	// Return the index of the new memory capability.
	return j;
}

/**
 * Revokes a memory capability and its children.
 */
int mem_revoke(pid_t owner, index_t i)
{
	if (UNLIKELY(!mem_valid_access(owner, i))) {
		return ERR_INVALID_ACCESS;
	}

	// Revoke the child capabilities.
	while (mem_table[i].cfree < mem_table[i].csize) {
		// Get the next child capability index.
		index_t j = i + mem_table[i].cfree;

		// Clear the PMP slot if it is set.
		if (mem_table[j].slot != 0) {
			proc_pmp_clear(mem_table[j].owner, mem_table[j].slot - 1);
		}
		// Invalidate the child capability.
		mem_table[j].owner = INVALID_PID;

		// Reclaim the child's capability table.
		mem_table[i].cfree += mem_table[j].cfree;

		if (UNLIKELY(preempt()))
			break;
	}

	// Return the number of unrevoked capabilities.
	return mem_table[i].csize - mem_table[i].cfree;
}

/**
 * Deletes a memory capability by invalidating its process ID.
 */
int mem_delete(pid_t owner, index_t i)
{
	if (UNLIKELY(!mem_valid_access(owner, i))) {
		return ERR_INVALID_ACCESS;
	}

	// Clear the PMP slot if it is set.
	if (mem_table[i].slot != 0) {
		proc_pmp_clear(owner, mem_table[i].slot - 1);
		mem_table[i].slot = 0;
	}

	// Invalidate the capability.
	mem_table[i].owner = INVALID_PID;

	return ERR_SUCCESS;
}

/**
 * Enables a memory capability by setting the PMP slot.
 */
int mem_pmp_set(pid_t owner, index_t i, pmp_slot_t slot, word_t rwx, word_t addr)
{
	if (UNLIKELY(!mem_valid_access(owner, i))) {
		return ERR_INVALID_ACCESS;
	}

	// Validate PMP arguments.
	if (UNLIKELY(!_valid_pmp_args(mem_table[i], slot, rwx, addr))) {
		return ERR_INVALID_ARGUMENT;
	}

	// Check if the slot is already in use.
	if (UNLIKELY(proc_pmp_is_set(owner, slot - 1))) {
		return ERR_SLOT_IN_USE;
	}

	// Clear the existing PMP slot if set.
	if (mem_table[i].slot != 0) {
		proc_pmp_clear(owner, mem_table[i].slot - 1);
	}

	// Set the new PMP slot and update the memory table.
	proc_pmp_set(owner, slot - 1, rwx, addr);
	mem_table[i].slot = slot;

	return ERR_SUCCESS;
}

/**
 * Retrieves the PMP configuration for a memory capability.
 */
int mem_pmp_get(pid_t owner, index_t i, pmp_slot_t *slot, mem_perm_t *rwx, pmp_addr_t *addr)
{
	if (UNLIKELY(!mem_valid_access(owner, i))) {
		return ERR_INVALID_ACCESS;
	}

	// If no PMP slot is set.
	if (mem_table[i].slot == 0) {
		*slot = 0;
		*rwx = 0;
		*addr = 0;
		return ERR_SUCCESS;
	}

	// Retrieve the PMP configuration.
	*slot = mem_table[i].slot;
	proc_pmp_get(owner, mem_table[i].slot - 1, rwx, addr);

	return ERR_SUCCESS;
}

/**
 * Disables a memory capability by clearing the PMP slot.
 */
int mem_pmp_clear(pid_t owner, index_t i)
{
	if (UNLIKELY(!mem_valid_access(owner, i))) {
		return ERR_INVALID_ACCESS;
	}

	// Clear the PMP slot if it is set.
	if (mem_table[i].slot != 0) {
		proc_pmp_clear(owner, mem_table[i].slot - 1);
		mem_table[i].slot = 0;
	}

	return ERR_SUCCESS;
}
