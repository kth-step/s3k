/**
 * @file cnode.h
 * @brief Functions for handling capabilities tree.
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */
#ifndef __CNODE_H__
#define __CNODE_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "cap.h"

/// @defgroup cnode Capability Node
///
/// Kernel internal module for handling capability tree.
///
/// The capability tree is not an actual tree, it is a linked list backed by an
/// array. The tree properties exists implicitly by the relations of
/// capabilities.
///
/// @{

typedef unsigned long cnode_handle_t;

/**
 * @brief Initialize the cnode structure.
 */
void cnode_init(void);

/**
 * @brief Get the handle for capability i of a process.
 * @param pid Process ID of the process.
 * @param idx Index.
 * @return handle for capability i of process pid.
 */
cnode_handle_t cnode_get_handle(cnode_handle_t pid, cnode_handle_t idx);

/**
 * @brief Get the Process ID of the handle.
 * @param handle Handle
 * @return PID of handle
 */
cnode_handle_t cnode_get_pid(cnode_handle_t handle);

/**
 * @brief Get handle of the root node.
 * @return handle to the root node.
 */
cnode_handle_t cnode_get_root_handle(void);

/**
 * @brief Get the next node after n.
 * @param handle Handle for node n.
 * @return _handle to the next node.
 */
cnode_handle_t cnode_get_next(cnode_handle_t handle);

/**
 * @brief Get capability at node n.
 * @param handle Handle for node n.
 * @return Capability.
 */
union cap cnode_get_cap(cnode_handle_t handle);

/**
 * @brief Set capability at node n.
 * @param handle Handle for node n.
 * @param cap Cap to put in node n.
 */
void cnode_set_cap(cnode_handle_t handle, union cap cap);

/**
 * @brief Checks if capability tree contains node n.
 * @param handle Handle for node n.
 * @return true if capability is present.
 */
bool cnode_contains(cnode_handle_t handle);

/**
 * @brief Insert a capability at node n after node m.
 * @param handle Handle to node n.
 * @param cap Capability to insert.
 * @param prev_handle Handle to node m.
 */
void cnode_insert(cnode_handle_t handle, union cap cap, cnode_handle_t prev_handle);

/**
 * @brief Move capability from node n to node m.
 * @param src_handle Handle to node n.
 * @param dst_handle Handle to node m.
 */
void cnode_move(cnode_handle_t src_handle, cnode_handle_t dstHandle);

/**
 * @brief Delete capability at n.
 * @param handle Handle to node n.
 */
void cnode_delete(cnode_handle_t handle);

/**
 * @brief Delete capability at n if m is the predecessor.
 * @param handle Handle to node n.
 * @param handle Handle to node m.
 */
bool cnode_delete_if(cnode_handle_t handle, cnode_handle_t prev_handle);
/// @}
#endif /* __CNODE_H__ */
