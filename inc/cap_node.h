// See LICENSE file for copyright and license details.
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "atomic.h"
#include "cap.h"
#include "kassert.h"
#include "preemption.h"

typedef struct cap_node cap_node_t;

struct cap_node {
        cap_node_t *prev, *next;
        cap_t cap;
};

extern cap_node_t cap_tables[N_PROC][N_CAPS];

static inline bool cap_node_is_deleted(cap_node_t* cn);
static inline cap_t cap_node_get_cap(cap_node_t* cn);

/* Delete node */
static inline bool cap_node_delete(cap_node_t* node);
/* Delete node iff node->prev == prev */
static inline bool cap_node_delete2(cap_node_t* node, cap_node_t* prev);
/* Insert node after parent, if insertion is successful, set the data to cap */
static inline bool cap_node_insert(cap_t cap, cap_node_t* node, cap_node_t* parent);
static inline bool cap_node_move(cap_t cap, cap_node_t* src_node, cap_node_t* dest_node);

/* Check if a node has been deleted */
bool cap_node_is_deleted(cap_node_t* cn)
{
        return cn->prev == NULL;
}

cap_t cap_node_get_cap(cap_node_t* cn)
{
        cap_t cap = cn->cap;
        synchronize();
        if (cap_node_is_deleted(cn))
                return NULL_CAP;
        return cap;
}

bool cap_node_delete(cap_node_t* node)
{
        cap_node_t* prev;
        do {
                prev = node->prev;
                if (prev == NULL)
                        return false;
        } while (!cap_node_delete2(node, prev));
        return true;
}

bool cap_node_delete2(cap_node_t* node, cap_node_t* prev)
{
        cap_node_t* next;
        if (!compare_and_swap(&node->prev, prev, NULL))
                return false;
        do {
                next = node->next;
        } while (!compare_and_swap(&next->prev, node, prev));
        prev->next = next;
        return true;
}

/**
 * Insert a child capability after the parent
 * only if the parent is not deleted.
 */
bool cap_node_insert(cap_t cap, cap_node_t* node, cap_node_t* prev)
{
        kassert(node->prev == NULL);
        node->cap = cap;
        cap_node_t* next = prev->next;
        while (prev->prev != NULL) {
                node->next = next;
                if (compare_and_swap(&next->prev, prev, node)) {
                        prev->next = node;
                        node->prev = prev;
                        return true;
                }
                next = prev->next;
        }
        return false;
}

bool cap_node_move(cap_t cap, cap_node_t* src_node, cap_node_t* dest_node)
{
        return cap_node_insert(cap, dest_node, src_node) && cap_node_delete(src_node);
}
