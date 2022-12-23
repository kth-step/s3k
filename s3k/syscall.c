// See LICENSE file for copyright and license details.

#include "syscall.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "cap_node.h"
#include "consts.h"
#include "csr.h"
#include "kprint.h"
#include "lock.h"
#include "preemption.h"
#include "proc.h"
#include "proc_state.h"
#include "sched.h"
#include "time.h"
#include "trap.h"

/*** INTERNAL FUNCTION DECLARATIONS ***/
/* For moving capability between processes */
static uint64_t interprocess_move(proc_t* src_proc, cap_node_t* src, proc_t* dest_proc, cap_node_t* dest);

/* Functions used when capability is modified, deleted, created. */
static void cap_update_hook(proc_t* proc, cap_node_t* node, const cap_t* cap);
static void cap_derive_upd(cap_t* cap, const cap_t* new_cap);
static void cap_reset(cap_t* cap);

/* Auxilary functions */
static inline bool sup_valid_pid(const cap_t* cap, uint64_t pid)
{
        kassert(cap_is_type(cap, CAP_SUPERVISOR));
        return cap_supervisor_get_free(cap) <= pid && pid < cap_supervisor_get_end(cap);
}

/*** SYSTEM CALLS ***/
uint64_t syscall_get_pid(void)
{
        current->regs.a0 = current->pid;
        return S3K_OK;
}

uint64_t syscall_get_reg(uint64_t reg)
{
        if (reg < N_REGISTERS) {
                current->regs.a0 = proc_get_reg(current, reg);
                return S3K_OK;
        }
        return S3K_ERROR;
}

uint64_t syscall_set_reg(uint64_t reg, uint64_t val)
{
        if (reg < N_REGISTERS) {
                current->regs.a0 = proc_set_reg(current, reg, val);
                return S3K_OK;
        }
        return S3K_ERROR;
}

void syscall_yield(void)
{
        uint64_t hartid = read_csr(mhartid);
        current->regs.timeout = read_timeout(hartid);
        sched_yield();
}

uint64_t syscall_read_cap(uint64_t cidx)
{
        kassert(current != NULL);
        cap_node_t* node = proc_get_cap_node(current, cidx);
        cap_t cap = cap_node_get_cap(node);

        current->regs.a0 = cap.word0;
        current->regs.a1 = cap.word1;
        return S3K_OK;
}

uint64_t syscall_move_cap(uint64_t src_cidx, uint64_t dest_cidx)
{
        kassert(current != NULL);
        cap_node_t* src_node = proc_get_cap_node(current, src_cidx);
        cap_node_t* dest_node = proc_get_cap_node(current, dest_cidx);
        if (cap_node_is_deleted(src_node))
                return S3K_EMPTY;
        if (!cap_node_is_deleted(dest_node))
                return S3K_OCCUPIED;
        if (cap_node_move(src_node, dest_node))
                return S3K_OK;
        return S3K_EMPTY;
}

uint64_t syscall_delete_cap(uint64_t cidx)
{
        kassert(current != NULL);
        cap_node_t* node = proc_get_cap_node(current, cidx);
        cap_t cap = cap_node_get_cap(node);

        cap_update_hook(NULL, node, &cap);
        if (cap_node_delete(node))
                return S3K_OK;
        return S3K_EMPTY;
}

uint64_t syscall_revoke_cap(uint64_t cidx)
{
        kassert(current != NULL);
        /* Return S3K_PREEMPTED if preempted */
        current->regs.a0 = S3K_PREEMPTED;

        /* !!! ENABLE PREEMPTION !!! */
        preemption_enable();

        /* Get the current node and capability*/
        cap_node_t* node = proc_get_cap_node(current, cidx);
        cap_t cap = node->cap;

        if (cap_node_is_deleted(node))
                return S3K_EMPTY;
        while (!cap_node_is_deleted(node)) {
                cap_node_t* next_node = node->next;
                cap_t next_cap = next_node->cap;
                if (!cap_is_child(&cap, &next_cap))
                        break;
                preemption_disable();
                if (cap_node_delete2(next_node, node))
                        cap_update_hook(current, node, &next_cap);
                preemption_enable();
        }

        preemption_disable();
        cap_reset(&node->cap);
        return S3K_OK;
}

uint64_t syscall_derive_cap(uint64_t src_cidx, uint64_t dest_cidx, uint64_t word0, uint64_t word1)
{
        kassert(current != NULL);

        /* If we get preempted, error code is S3K_PREEMPTED */
        current->regs.a0 = S3K_PREEMPTED;

        /* !!! ENABLE PREEMPTION !!! */
        preemption_enable();

        cap_node_t* src_node = proc_get_cap_node(current, src_cidx);
        cap_node_t* dest_node = proc_get_cap_node(current, dest_cidx);

        /* Check if we can derive the capability */
        if (cap_node_is_deleted(src_node))
                return S3K_EMPTY;
        if (!cap_node_is_deleted(dest_node))
                return S3K_OCCUPIED;
        dest_node->cap = (cap_t){word0, word1};
        if (!cap_can_derive(&src_node->cap, &dest_node->cap))
                return S3K_ILLEGAL_DERIVATION;

        /* !!! DISABLE PREEMPTION !!! */
        preemption_disable();
        cap_derive_upd(&src_node->cap, &dest_node->cap);
        if (cap_node_insert(dest_node, src_node)) {
                cap_update_hook(current, dest_node, &dest_node->cap);
                return S3K_OK;
        }
        return S3K_EMPTY;
}

uint64_t syscall_sup_suspend(uint64_t cidx, uint64_t pid)
{
        cap_node_t* node = proc_get_cap_node(current, cidx);
        cap_t cap = cap_node_get_cap(node);
        proc_t* supervisee = &processes[pid];
        if (!cap_is_type(&cap, CAP_SUPERVISOR))
                return S3K_INVALID_CAPABILITY;
        if (!sup_valid_pid(&cap, pid))
                return S3K_INVALID_SUPERVISEE;
        if (proc_supervisor_suspend(supervisee))
                return S3K_OK;
        return S3K_ERROR;
}

uint64_t syscall_sup_resume(uint64_t cidx, uint64_t pid)
{
        cap_node_t* node = proc_get_cap_node(current, cidx);
        cap_t cap = cap_node_get_cap(node);
        proc_t* supervisee = &processes[pid];
        if (!cap_is_type(&cap, CAP_SUPERVISOR))
                return S3K_INVALID_CAPABILITY;
        if (!sup_valid_pid(&cap, pid))
                return S3K_INVALID_SUPERVISEE;
        if (proc_supervisor_resume(supervisee))
                return S3K_OK;
        return S3K_ERROR;
}

uint64_t syscall_sup_get_state(uint64_t cidx, uint64_t pid)
{
        cap_node_t* node = proc_get_cap_node(current, cidx);
        cap_t cap = cap_node_get_cap(node);
        proc_t* supervisee = &processes[pid];
        if (!cap_is_type(&cap, CAP_SUPERVISOR))
                return S3K_INVALID_CAPABILITY;
        if (!sup_valid_pid(&cap, pid))
                return S3K_INVALID_SUPERVISEE;
        current->regs.a0 = supervisee->state;
        return S3K_OK;
}

uint64_t syscall_sup_get_reg(uint64_t cidx, uint64_t pid, uint64_t reg)
{
        cap_node_t* node = proc_get_cap_node(current, cidx);
        cap_t cap = cap_node_get_cap(node);
        proc_t* supervisee = &processes[pid];
        if (!cap_is_type(&cap, CAP_SUPERVISOR))
                return S3K_INVALID_CAPABILITY;
        if (!sup_valid_pid(&cap, pid))
                return S3K_INVALID_SUPERVISEE;

        if (reg < N_REGISTERS) {
                current->regs.a0 = proc_get_reg(supervisee, reg);
                return S3K_OK;
        }
        return S3K_ERROR;
}

uint64_t syscall_sup_set_reg(uint64_t cidx, uint64_t pid, uint64_t reg, uint64_t val)
{
        cap_node_t* node = proc_get_cap_node(current, cidx);
        cap_t cap = cap_node_get_cap(node);
        proc_t* supervisee = &processes[pid];
        if (!cap_is_type(&cap, CAP_SUPERVISOR))
                return S3K_INVALID_CAPABILITY;
        if (!sup_valid_pid(&cap, pid))
                return S3K_INVALID_SUPERVISEE;
        if (!proc_supervisor_acquire(supervisee))
                return S3K_SUPERVISEE_BUSY;
        if (reg < N_REGISTERS) {
                current->regs.a0 = proc_set_reg(supervisee, reg, val);
                proc_supervisor_release(supervisee);
                return S3K_OK;
        }
        proc_supervisor_release(supervisee);
        return S3K_ERROR;
}

uint64_t syscall_sup_read_cap(uint64_t cidx, uint64_t pid, uint64_t read_cidx)
{
        cap_node_t* node = proc_get_cap_node(current, cidx);
        cap_t cap = cap_node_get_cap(node);
        proc_t* supervisee = &processes[pid];
        if (!cap_is_type(&cap, CAP_SUPERVISOR))
                return S3K_INVALID_CAPABILITY;
        if (!sup_valid_pid(&cap, pid))
                return S3K_INVALID_SUPERVISEE;
        if (!proc_supervisor_acquire(supervisee))
                return S3K_SUPERVISEE_BUSY;

        cap_t read_cap = cap_node_get_cap(proc_get_cap_node(supervisee, read_cidx));
        current->regs.a0 = read_cap.word0;
        current->regs.a1 = read_cap.word1;

        proc_supervisor_release(supervisee);

        return S3K_OK;
}

uint64_t syscall_sup_move_cap(uint64_t cidx, uint64_t pid, uint64_t take, uint64_t src, uint64_t dest)
{
        cap_node_t* node = proc_get_cap_node(current, cidx);
        cap_t cap = cap_node_get_cap(node);
        proc_t* supervisee = &processes[pid];
        if (!cap_is_type(&cap, CAP_SUPERVISOR))
                return S3K_INVALID_CAPABILITY;
        if (!sup_valid_pid(&cap, pid))
                return S3K_INVALID_SUPERVISEE;
        if (!proc_supervisor_acquire(supervisee))
                return S3K_SUPERVISEE_BUSY;

        s3k_code_t code;
        cap_node_t *src_node, *dest_node;
        if (take) {
                src_node = proc_get_cap_node(supervisee, src);
                dest_node = proc_get_cap_node(current, dest);
                code = interprocess_move(supervisee, src_node, current, dest_node);
        } else {
                src_node = proc_get_cap_node(current, src);
                dest_node = proc_get_cap_node(supervisee, dest);
                code = interprocess_move(current, src_node, supervisee, dest_node);
        }
        proc_supervisor_release(supervisee);
        return code;
}

/*** INTERNAL FUNCTIONS ***/

uint64_t interprocess_move(proc_t* src_proc, cap_node_t* src, proc_t* dest_proc, cap_node_t* dest)
{
        if (cap_node_is_deleted(src))
                return S3K_EMPTY;
        if (!cap_node_is_deleted(dest))
                return S3K_OCCUPIED;
        cap_update_hook(dest_proc, src, &src->cap);
        return cap_node_move(src, dest) ? S3K_OK : S3K_EMPTY;
}

void cap_update_hook(proc_t* proc, cap_node_t* node, const cap_t* cap)
{
        if (cap_is_type(cap, CAP_TIME)) {
                uint64_t hartid = cap_time_get_hartid(cap);
                uint64_t free = cap_time_get_free(cap);
                uint64_t end = cap_time_get_end(cap);
                uint64_t pid = proc ? proc->pid : INVALID_PID;
                sched_update(node, hartid, free, end, pid);
        }
}

void cap_reset(cap_t* cap)
{
        if (cap_is_type(cap, CAP_MEMORY)) {
                cap_memory_set_lock(cap, 0);
                cap_memory_set_free(cap, cap_memory_get_begin(cap));
        } else if (cap_is_type(cap, CAP_TIME)) {
                cap_time_set_free(cap, cap_time_get_begin(cap));
        } else if (cap_is_type(cap, CAP_PORTS)) {
                cap_time_set_free(cap, cap_time_get_begin(cap));
        } else if (cap_is_type(cap, CAP_SUPERVISOR)) {
                cap_supervisor_set_free(cap, cap_supervisor_get_begin(cap));
        }
}

void cap_derive_upd(cap_t* src_cap, const cap_t* new_cap)
{
        if (cap_is_type(src_cap, CAP_MEMORY)) {
                if (cap_is_type(new_cap, CAP_MEMORY)) {
                        cap_memory_set_free(src_cap, cap_memory_get_end(new_cap));
                } else {
                        cap_memory_set_lock(src_cap, 1);
                }
        } else if (cap_is_type(src_cap, CAP_TIME)) {
                cap_time_set_free(src_cap, cap_time_get_end(new_cap));
        } else if (cap_is_type(src_cap, CAP_PORTS)) {
                if (cap_is_type(new_cap, CAP_PORTS)) {
                        cap_ports_set_free(src_cap, cap_ports_get_end(new_cap));
                }
        }
}
