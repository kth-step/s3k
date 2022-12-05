// See LICENSE file for copyright and license details.
#pragma once
#include <stdint.h>

#include "cap_node.h"
#include "proc.h"

/* Without capabilities */
uint64_t syscall_get_pid(void);
uint64_t syscall_get_reg(uint64_t regnr);
uint64_t syscall_set_reg(uint64_t regnr, uint64_t val);
void syscall_yield(void) __attribute__((noreturn));
/* Basic capabilities */
uint64_t syscall_read_cap(uint64_t cidx);
uint64_t syscall_move_cap(uint64_t src_cidx, uint64_t dest_cidx);
uint64_t syscall_delete_cap(uint64_t cidx);
uint64_t syscall_revoke_cap(uint64_t cidx);
uint64_t syscall_derive_cap(uint64_t src_cidx, uint64_t dest_cidx, uint64_t word0, uint64_t word1);
/* Superviser capabilities functions */
uint64_t syscall_sup_suspend(uint64_t cidx, uint64_t pid);
uint64_t syscall_sup_resume(uint64_t cidx, uint64_t pid);
uint64_t syscall_sup_get_state(uint64_t cidx, uint64_t pid);
uint64_t syscall_sup_get_reg(uint64_t cidx, uint64_t pid, uint64_t reg);
uint64_t syscall_sup_set_reg(uint64_t cidx, uint64_t pid, uint64_t reg, uint64_t val);
uint64_t syscall_sup_read_cap(uint64_t cidx, uint64_t pid, uint64_t read_cidx);
uint64_t syscall_sup_move_cap(uint64_t cidx, uint64_t pid, uint64_t take, uint64_t src_cidx, uint64_t dest_cidx);
