#pragma once
/**
 * @file proc.h
 * @brief Defines the process control block and its associated functions.
 *
 * This file contains the definition of the `proc_t` data structure, which
 * represents a process control block (PCB) in the operating system. It also
 * contains the declarations of functions for manipulating the PCB.
 *
 * @copyright MIT License
 */

#include "kern/types.h"

typedef struct regs regs_t;
typedef struct pmp pmp_t;
typedef struct proc proc_t;
typedef enum proc_state_flag proc_state_flag_t;

struct regs {
	uintptr_t pc, ra, sp, gp, tp;
	uintptr_t t0, t1, t2;
	uintptr_t s0, s1;
	uintptr_t a0, a1, a2, a3, a4, a5, a6, a7;
	uintptr_t s2, s3, s4, s5, s6, s7, s8, s9, s10, s11;
	uintptr_t t3, t4, t5, t6;
	uintptr_t tpc, tsp;
	uintptr_t epc, esp, ecause, eval;
	uintptr_t servtime;
};

struct pmp {
	uint8_t cfg[8];
	uintptr_t addr[8];
};

enum proc_state_flag {
	PSF_BUSY = 1,
	PSF_BLOCKED = 2,
	PSF_SUSPENDED = 4,
};

struct proc {
	regs_t regs;
	pmp_t pmp;
	proc_state_t state;
	uintptr_t pid;
	uint64_t timeout;
};

register proc_t *const current __asm__("tp");

/**
 * Initializes all processes in the system.
 *
 * @note This function should be called only once during system startup.
 */
void proc_init(void);

/**
 * @brief Gets the process corresponding to a given process ID.
 *
 * @param pid The process ID to look for.
 * @return A pointer to the process corresponding to the given PID.
 */
proc_t *proc_get(pid_t pid);

proc_state_t proc_get_state(proc_t *proc);

bool proc_acquire(proc_t* proc);
void proc_release(proc_t* proc);
void proc_suspend(proc_t* proc);
void proc_resume(proc_t* proc);
void proc_ipc_wait(proc_t* proc, chan_t chan);
bool proc_ipc_acquire(proc_t* proc, chan_t chan);
bool proc_is_suspended(proc_t* proc);

bool proc_pmp_avail(proc_t* proc, pmp_slot_t slot);
void proc_pmp_load(proc_t* proc, pmp_slot_t slot, rwx_t cfg, napot_t addr);
void proc_pmp_unload(proc_t* proc, pmp_slot_t slot);
