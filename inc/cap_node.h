/* See LICENSE file for copyright and license details. */
#ifndef __CAP_NODE_H__
#define __CAP_NODE_H__
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "cap.h"

struct cap_node {
	struct cap_node *prev;
	struct cap_node *next;
	struct cap cap;
};

bool cap_node_get(const struct cap_node *node, struct cap *cap);
void cap_node_set(struct cap_node *node, const struct cap *cap);
bool cap_node_deleted(const struct cap_node *node);
uint64_t cap_node_insert(struct cap_node *node, struct cap_node *prev);
uint64_t cap_node_move(struct cap_node *src, struct cap_node *dest);
uint64_t cap_node_delete(struct cap_node *node);
uint64_t cap_node_delete2(struct cap_node *node, struct cap_node *prev);
#endif /* __CAP_NODE_H__ */
