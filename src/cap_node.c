/* See LICENSE file for copyright and license details. */
#include "cap_node.h"

#include "cap.h"
#include "common.h"
#include "consts.h"

bool cap_node_deleted(const struct cap_node *node)
{
	return node->prev == NULL;
}

bool cap_node_get(const struct cap_node *node, struct cap *cap)
{
	cap->word0 = node->cap.word0;
	cap->word1 = node->cap.word1;
	__atomic_thread_fence(__ATOMIC_SEQ_CST);
	if (cap_node_deleted(node))
		return false;
	return true;
}

void cap_node_set(struct cap_node *node, const struct cap *cap)
{
	node->cap.word0 = cap->word0;
	node->cap.word1 = cap->word1;
}

uint64_t cap_node_insert(struct cap_node *node, struct cap_node *prev)
{
	struct cap_node *next = prev->next;
	node->prev = prev;
	node->next = next;
	next->prev = node;
	prev->next = node;
	return EXCPT_NONE;
}

uint64_t cap_node_delete(struct cap_node *node)
{
	return cap_node_delete2(node, node->prev);
}

uint64_t cap_node_delete2(struct cap_node *node, struct cap_node *prev)
{
	struct cap_node *next;
	next = node->next;
	prev = node->prev;
	prev->next = next;
	next->prev = prev;
	node->prev = NULL;
	node->next = NULL;
	return EXCPT_NONE;
}

uint64_t cap_node_move(struct cap_node *node, struct cap_node *dest)
{
	dest->cap = node->cap;
	struct cap_node *next, *prev;
	next = node->next;
	prev = node->prev;
	dest->prev = prev;
	dest->next = next;
	prev->next = dest;
	next->prev = dest;
	node->prev = NULL;
	node->next = NULL;
	return EXCPT_UNIMPLEMENTED;
}
