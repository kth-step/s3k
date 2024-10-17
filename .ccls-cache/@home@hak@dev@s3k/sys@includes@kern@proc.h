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

#define REGISTER_COUNT ((sizeof(Registers) / sizeof(Word)))

typedef unsigned long Pid;
typedef unsigned long ProcState;

typedef struct {
	Word pc, ra, sp, gp, tp;
	Word t0, t1, t2;
	Word s0, s1;
	Word a0, a1, a2, a3, a4, a5, a6, a7;
	Word s2, s3, s4, s5, s6, s7, s8, s9, s10, s11;
	Word t3, t4, t5, t6;
	Word tpc, tsp;
	Word epc, esp, ecause, eval;
	Word servtime;
} Registers;

typedef struct {
	Word cfg;
	Word addr[8];
} PmpConf;

typedef enum {
	PSF_BUSY = 1,
	PSF_BLOCKED = 2,
	PSF_SUSPENDED = 4,
} ProcStateFlag;

typedef struct {
	Registers regs;
	PmpConf pmp;
	ProcState state;
	Pid pid;
	Time timeout;
} Proc;

register Proc *const current __asm__("tp");

/**
 * Initializes all processes in the system.
 *
 * @note This function should be called only once during system startup.
 */
void ProcInit(void);

/**
 * @brief Gets the process corresponding to a given process ID.
 *
 * @param pid The process ID to look for.
 * @return A pointer to the process corresponding to the given PID.
 */
Proc *ProcGet(Pid pid);

ProcState ProcGetState(Proc *proc);

bool ProcAcquire(Proc *proc);
void ProcRelease(Proc *proc);
void ProcSuspend(Proc *proc);
void ProcResume(Proc *proc);
void ProcIpcWait(Proc *proc, Word chan);
bool ProcIpcAcquire(Proc *proc, Word chan);
bool ProcIsSuspended(Proc *proc);
