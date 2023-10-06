/**
 * @file cap_monitor.h
 * @brief Capability Monitor Interface
 *
 * This file provides the interface for monitoring and managing processes
 * through capabilities. It includes functions for suspending and resuming
 * processes, reading and writing to process registers, and managing process
 * capabilities and PMP configurations.
 *
 * The functions provided require the processes to be in a suspended state for
 * operations to be performed. Error codes are returned for unauthorized,
 * invalid, or inappropriate requests.
 */

#pragma once
#include "cap_table.h"
#include "error.h"
#include "proc.h"

/**
 * Suspends a specified process.
 *
 * @param mon The CTE of the monitor capability.
 * @param pid The ID of the process to be suspended.
 * @return SUCCESS if the process set to suspend.
 *         ERR_EMPTY if the CTE is empty.
 *         ERR_INVALID_MONITOR if the CTE has the wrong capability type or
 * authorization.
 */
err_t cap_monitor_suspend(cte_t mon, pid_t pid);

/**
 * Resumes a specified suspended process.
 *
 * @param mon The CTE of the monitor capability.
 * @param pid The ID of the process to be resumed.
 * @return SUCCESS if the process is resumed.
 *         ERR_EMPTY if the CTE is empty.
 *         ERR_INVALID_MONITOR if the CTE has the wrong capability type or is
 * unauthorized.
 */
err_t cap_monitor_resume(cte_t mon, pid_t pid);

/**
 * Reads a register value from a specified suspended process.
 *
 * @param mon The CTE of the monitor capability.
 * @param pid The ID of the process whose register is to be read.
 * @param reg The register to be read.
 * @param val Pointer to store the read value.
 * @return SUCCESS if the register is read.
 *         ERR_EMPTY if the CTE is empty.
 *         ERR_INVALID_MONITOR if unauthorized or wrong capability type.
 *         ERR_INVALID_STATE if the process is not suspended.
 */
err_t cap_monitor_reg_read(cte_t mon, pid_t pid, reg_t reg, uint64_t *val);

/**
 * Writes a value to a specified register of a suspended process.
 *
 * @param mon The CTE of the monitor capability.
 * @param pid The ID of the process whose register is to be written.
 * @param reg The register to be written to.
 * @param val The value to write.
 * @return SUCCESS if the value is written.
 *         ERR_EMPTY if the CTE is empty.
 *         ERR_INVALID_MONITOR if unauthorized or wrong capability type.
 *         ERR_INVALID_STATE if the process is not suspended.
 */
err_t cap_monitor_reg_write(cte_t mon, pid_t pid, reg_t reg, uint64_t val);

/**
 * Reads a capability from a specified source CTE of a suspended process.
 *
 * @param mon The CTE of the monitor capability.
 * @param src The source CTE.
 * @param cap Pointer to store the read capability.
 * @return SUCCESS if the capability is read.
 *         ERR_EMPTY if the CTE is empty.
 *         ERR_INVALID_MONITOR if unauthorized or wrong capability type.
 *         ERR_INVALID_STATE if the process is not suspended.
 */
err_t cap_monitor_cap_read(cte_t mon, cte_t src, cap_t *cap);

/**
 * Moves a capability from a source to a destination CTE of a suspended process.
 *
 * @param mon The CTE of the monitor capability.
 * @param src The source CTE.
 * @param dst The destination CTE.
 * @return SUCCESS if the capability is moved.
 *         ERR_EMPTY if the CTE is empty.
 *         ERR_INVALID_MONITOR if unauthorized or wrong capability type.
 *         ERR_INVALID_STATE if the process is not suspended.
 */
err_t cap_monitor_cap_move(cte_t mon, cte_t src, cte_t dst);

/**
 * Loads a PMP configuration into a specified slot of a suspended process.
 *
 * @param mon The CTE of the monitor capability.
 * @param pmp The CTE of the PMP capability.
 * @param pmp_slot The slot to load the PMP configuration into.
 * @return SUCCESS if the configuration is loaded.
 *         ERR_EMPTY if the CTE is empty.
 *         ERR_INVALID_MONITOR if unauthorized or wrong capability type.
 *         ERR_INVALID_STATE if the process is not suspended.
 */
err_t cap_monitor_pmp_load(cte_t mon, cte_t pmp, pmp_slot_t pmp_slot);

/**
 * Unloads a PMP configuration from a specified PMP CTE of a suspended process.
 *
 * @param mon The CTE of the monitor capability.
 * @param pmp The CTE of the PMP capability to unload.
 * @return SUCCESS if the configuration is unloaded.
 *         ERR_EMPTY if the CTE is empty.
 *         ERR_INVALID_MONITOR if unauthorized or wrong capability type.
 *         ERR_INVALID_STATE if the process is not suspended.
 */
err_t cap_monitor_pmp_unload(cte_t mon, cte_t pmp);
