#ifndef CAP_MEM_H
#define CAP_MEM_H

#include "types.h"

typedef struct {
	pid_t owner;	 ///< Process ID of the owner of the capability.
	fuel_t cfree;	 ///< Remaining cfree for the capability.
	fuel_t csize;	 ///< Initial cfree allocated to the capability.
	pmp_slot_t slot; ///< PMP slot used.
	mem_perm_t rwx;	 ///< Permissions (Read, Write, Execute) encoded as bits.
	mem_addr_t base; ///< Start address of the memory region.
	mem_addr_t size; ///< End address of the memory region.
} __attribute__((aligned(sizeof(word_t)))) mem_t;

/**
 * @brief Initializes the memory subsystem.
 *
 * @param init_mems Array of initial memory capabilities.
 */
void mem_init(mem_t init_mems[]);

/**
 * @brief Checks if the memory capability is valid for the given owner and index.
 *
 * This function checks if the memory capability at the given index is valid
 * for the specified owner process.
 *
 * @param owner The owner of the capability.
 * @param i The index of the capability.
 * @return true if valid, false otherwise.
 */
bool mem_valid_access(pid_t owner, index_t i);

/**
 * Transfer a memory capability from one process to another.
 *
 * @param owner The process ID of the current owner of the memory capability.
 * @param index The index in the memory table of the capability to be transferred.
 * @param new_owner The process ID of the new owner of the memory capability.
 * @return ERR_SUCCESS if the capability is successfully granted,
 *         ERR_INVALID_ACCESS if the owner does not match the entry in the memory table.
 */
int mem_transfer(pid_t owner, index_t index, pid_t new_owner);

/**
 * Retrieves a memory capability from the memory table.
 *
 * @param owner The process ID associated with the memory capability.
 * @param index The index in the memory table.
 * @param mem_t A pointer to store the retrieved memory capability.
 * @return ERR_SUCCESS if the memory capability is successfully retrieved,
 *         ERR_INVALID_ACCESS if the owner does not match the entry in the memory table.
 */
int mem_get(pid_t owner, index_t i, mem_t *mem_t);

/**
 * Derives a new memory capability from an existing one.
 *
 * @param owner The process ID associated with the existing memory capability.
 * @param index The index in the memory table of the existing capability.
 * @param child_pid The process ID for the new capability.
 * @param child_fuel The amount of cfree for the new capability.
 * @param child_rwx The permissions for the new capability.
 * @param child_base The start address of the new capability's memory region.
 * @param child_size The end address of the new capability's memory region.
 * @return The index of the new memory capability if successfully derived,
 *         ERR_INVALID_ACCESS if the owner does not match the entry in the memory table,
 *         ERR_INVALID_ARGUMENT if the new memory capability cannot be derived.
 */
int mem_derive(pid_t owner, index_t i, pid_t child_pid, fuel_t child_fuel, mem_perm_t child_rwx, mem_addr_t child_base,
	       mem_addr_t child_size);

/**
 * Revokes a memory capability and its children.
 *
 * @param owner The process ID associated with the memory capability.
 * @param index The index in the memory table.
 * @return ERR_SUCCESS if the memory capability is successfully revoked,
 *         ERR_INVALID_ACCESS if the owner does not match the entry in the memory table.
 */
int mem_revoke(pid_t owner, index_t i);

/**
 * Deletes a memory capability by invalidating its process ID.
 *
 * @param owner The process ID associated with the memory capability.
 * @param index The index in the memory table.
 * @return ERR_SUCCESS if the memory capability is successfully deleted,
 *         ERR_INVALID_ACCESS if the owner does not match the entry in the memory table.
 */
int mem_delete(pid_t owner, index_t i);

/**
 * Enables a memory capability by setting a PMP slot.
 *
 * @param owner The process ID of the owner of the memory capability.
 * @param index The index in the memory table of the capability to be enabled.
 * @param slot The PMP slot to be set.
 * @param rwx The permissions for the PMP slot.
 * @param addr The address for the PMP slot.
 * @return ERR_SUCCESS if the capability is successfully enabled,
 *         ERR_INVALID_ACCESS if the owner does not match the entry in the memory table,
 *         ERR_INVALID_ARGUMENT if the PMP arguments are invalid,
 *         ERR_SLOT_IN_USE if the PMP slot is already in use.
 */
int mem_pmp_set(pid_t owner, index_t i, pmp_slot_t slot, word_t rwx, word_t addr);

/**
 * Retrieves the PMP configuration for a memory capability.
 *
 * @param owner The process ID of the owner of the memory capability.
 * @param index The index in the memory table of the capability.
 * @param slot A pointer to store the PMP slot.
 * @param rwx A pointer to store the permissions for the PMP slot.
 * @param addr A pointer to store the address for the PMP slot.
 * @return ERR_SUCCESS if the PMP configuration is successfully retrieved,
 *         ERR_INVALID_ACCESS if the owner does not match the entry in the memory table.
 */
int mem_pmp_get(pid_t owner, index_t i, pmp_slot_t *slot, mem_perm_t *rwx, pmp_addr_t *addr);

/**
 * Disables a memory capability by clearing its PMP slot.
 *
 * @param owner The process ID of the owner of the memory capability.
 * @param index The index in the memory table of the capability to be disabled.
 * @return ERR_SUCCESS if the capability is successfully disabled,
 *         ERR_INVALID_ACCESS if the owner does not match the entry in the memory table.
 */
int mem_pmp_clear(pid_t owner, index_t i);

#endif // CAP_MEM_H
