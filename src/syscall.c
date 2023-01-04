/* See LICENSE file for copyright and license details. */
#include "syscall.h"

#include "cap.h"
#include "common.h"
#include "consts.h"
#include "sched.h"

/*** Auxiliary functions ***/
struct cap_node *get_cap_node(struct proc *proc, uint64_t i)
{
	return (i < NCAP) ? &proc->caps[i] : NULL;
}

struct proc *setret(struct proc *proc, uint64_t code)
{
	proc->regs.a0 = code;
	return proc;
}

struct proc *revcap_time(struct proc *proc, struct cap_node *node, struct cap *cap)
{
	ASSERT(cap_get_type(&node->cap) == CAP_TIME);
	return proc;
}

struct proc *revcap_memory(struct proc *proc, struct cap_node *node, struct cap *cap)
{
	ASSERT(cap_get_type(&node->cap) == CAP_MEMORY);
	return proc;
}

struct proc *revcap_monitor(struct proc *proc, struct cap_node *node, struct cap *cap)
{
	ASSERT(cap_get_type(&node->cap) == CAP_MONITOR);
	return proc;
}

struct proc *drvcap_time(struct proc *proc, struct cap_node *pnode, struct cap_node *cnode)
{
	ASSERT(cap_get_type(&pnode->cap) == CAP_TIME);
	if (!cap_time_can_derive(&pnode->cap, &cnode->cap))
		return setret(proc, EXCPT_DERIVATION);
	uint64_t code = cap_node_insert(pnode, cnode);
	if (!code)
		return setret(proc, code);
	/* TODO: Update schedule */
	cap_time_set_free(&pnode->cap, cap_time_get_begin(&cnode->cap));
	return setret(proc, EXCPT_NONE);
}

struct proc *drvcap_memory(struct proc *proc, struct cap_node *pnode, struct cap_node *cnode)
{
	ASSERT(cap_get_type(&pnode->cap) == CAP_MEMORY);
	if (!cap_memory_can_derive(&pnode->cap, &cnode->cap))
		return setret(proc, EXCPT_DERIVATION);
	uint64_t code = cap_node_insert(pnode, cnode);
	if (!code)
		return setret(proc, code);
	cap_memory_set_free(&pnode->cap, cap_memory_get_begin(&cnode->cap));
	return setret(proc, EXCPT_NONE);
}

struct proc *drvcap_monitor(struct proc *proc, struct cap_node *pnode, struct cap_node *cnode)
{
	ASSERT(cap_get_type(&pnode->cap) == CAP_MONITOR);
	if (!cap_monitor_can_derive(&pnode->cap, &cnode->cap))
		return setret(proc, EXCPT_DERIVATION);
	uint64_t code = cap_node_insert(pnode, cnode);
	if (!code)
		return setret(proc, code);
	cap_monitor_set_free(&pnode->cap, cap_monitor_get_begin(&cnode->cap));
	return setret(proc, EXCPT_NONE);
}

struct proc *invcap_monitor(struct proc *proc, struct cap_node *mnode, uint64_t pid, uint64_t op,
			    uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3)
{
	ASSERT(cap_get_type(&mnode->cap) == CAP_MONITOR);
	if (pid < cap_monitor_get_free(&mnode->cap) && pid >= cap_monitor_get_end(&mnode->cap))
		return setret(proc, EXCPT_PID);
	return setret(proc, EXCPT_UNIMPLEMENTED);
}

/*** System call handlers ***/
struct proc *syscall_getpid(struct proc *proc)
{
	proc->regs.a0 = proc->pid;
	return proc;
}

struct proc *syscall_getreg(struct proc *proc, uint64_t reg)
{
	uint64_t *regs = (uint64_t *)&proc->regs;
	return setret(proc, reg < REG_COUNT ? regs[reg] : 0);
}

struct proc *syscall_setreg(struct proc *proc, uint64_t reg, uint64_t val)
{
	uint64_t *regs = (uint64_t *)&proc->regs;
	if (reg < REG_COUNT) {
		regs[reg] = val;
		return setret(proc, EXCPT_NONE);
	}
	return setret(proc, EXCPT_INDEX);
}

struct proc *syscall_yield(struct proc *proc)
{
	return schedule_yield(proc);
}

struct proc *syscall_getcap(struct proc *proc, uint64_t i)
{
	struct cap_node *node = get_cap_node(proc, i);
	struct cap *cap = (struct cap *)&proc->regs.a1;
	if (node == NULL)
		return setret(proc, EXCPT_INDEX);
	else if (!cap_node_get(node, cap))
		return setret(proc, EXCPT_EMPTY);
	return setret(proc, EXCPT_NONE);
}

struct proc *syscall_movcap(struct proc *proc, uint64_t i, uint64_t j)
{
	struct cap_node *inode = get_cap_node(proc, i);
	struct cap_node *jnode = get_cap_node(proc, j);
	if (inode == NULL || jnode == NULL)
		return setret(proc, EXCPT_INDEX);
	else if (cap_node_deleted(inode))
		return setret(proc, EXCPT_EMPTY);
	else if (!cap_node_deleted(jnode))
		return setret(proc, EXCPT_COLLISION);
	return setret(proc, cap_node_move(inode, jnode));
}

struct proc *syscall_delcap(struct proc *proc, uint64_t i)
{
	struct cap_node *inode = get_cap_node(proc, i);
	if (inode == NULL)
		return setret(proc, EXCPT_INDEX);
	else if (cap_node_deleted(inode))
		return setret(proc, EXCPT_EMPTY);
	return setret(proc, cap_node_delete(inode));
}

struct proc *syscall_revcap(struct proc *proc, uint64_t i)
{
	struct cap_node *inode = get_cap_node(proc, i);
	struct cap icap;
	if (inode == NULL)
		return setret(proc, EXCPT_INDEX);
	else if (!cap_node_get(inode, &icap))
		return setret(proc, EXCPT_EMPTY);

	switch (cap_get_type(&icap)) {
	case CAP_TIME:
		return revcap_time(proc, inode, &icap);
	case CAP_MEMORY:
		return revcap_memory(proc, inode, &icap);
	case CAP_MONITOR:
		return revcap_monitor(proc, inode, &icap);
	default:
		return setret(proc, EXCPT_UNIMPLEMENTED);
	}
}

struct proc *syscall_drvcap(struct proc *proc, uint64_t i, uint64_t j, uint64_t word0,
			    uint64_t word1)
{
	struct cap_node *inode = get_cap_node(proc, i);
	struct cap_node *jnode = get_cap_node(proc, j);

	if (inode == NULL || jnode == NULL)
		return setret(proc, EXCPT_INDEX);
	else if (cap_node_deleted(inode))
		return setret(proc, EXCPT_EMPTY);
	else if (!cap_node_deleted(jnode))
		return setret(proc, EXCPT_COLLISION);

	jnode->cap = (struct cap){word0, word1};
	switch (cap_get_type(&inode->cap)) {
	case CAP_TIME:
		return drvcap_time(proc, inode, jnode);
	case CAP_MEMORY:
		return drvcap_memory(proc, inode, jnode);
	case CAP_MONITOR:
		return drvcap_monitor(proc, inode, jnode);
	default:
		return setret(proc, EXCPT_UNIMPLEMENTED);
	}
}

struct proc *syscall_invcap(struct proc *proc, uint64_t i, uint64_t arg0, uint64_t arg1,
			    uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5)
{
	struct cap_node *inode = get_cap_node(proc, i);
	switch (cap_get_type(&inode->cap)) {
	case CAP_MONITOR:
		return invcap_monitor(proc, inode, arg0, arg1, arg2, arg3, arg4, arg5);
	default:
		return setret(proc, EXCPT_UNIMPLEMENTED);
	}
}
