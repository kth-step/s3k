/* See LICENSE file for copyright and license details. */
#ifndef __SYSCALL_H__
#define __SYSCALL_H__
#include <stdint.h>

#include "proc.h"

struct proc *syscall_getpid(struct proc *proc);
struct proc *syscall_getreg(struct proc *proc, uint64_t reg);
struct proc *syscall_setreg(struct proc *proc, uint64_t reg, uint64_t val);
struct proc *syscall_yield(struct proc *proc);
struct proc *syscall_getcap(struct proc *proc, uint64_t i);
struct proc *syscall_movcap(struct proc *proc, uint64_t i, uint64_t j);
struct proc *syscall_delcap(struct proc *proc, uint64_t i);
struct proc *syscall_revcap(struct proc *proc, uint64_t i);
struct proc *syscall_drvcap(struct proc *proc, uint64_t i, uint64_t j, uint64_t word0,
			    uint64_t word1);
struct proc *syscall_invcap(struct proc *proc, uint64_t i, uint64_t arg0, uint64_t arg1,
			    uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5);

#endif /* __SYSCALL_H__ */
