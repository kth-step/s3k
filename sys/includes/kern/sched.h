#pragma once

#include "kern/proc.h"

/**
 * @file sched.h
 * @brief Scheduler.
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 * @bug QEMU mret does not work properly if all pmp registers are 0, so we have
 * a temporary fix in sched_next.
 */

/**
 * @brief Initialize the scheduler.
 *
 * This function initializes the scheduler, which is responsible for managing
 * the execution of processes on the system. It sets up the necessary data
 * structures and configurations to support scheduling.
 */
void SchedInit(void);

/**
 * @brief Find the next process to schedule.
 *
 * This function finds the next process to schedule based on the current
 * state of the system.
 */
Proc *SchedNext(void);

/// Let pid run on hartid, begin-end.
void SchedUpdate(Word pid, Word hart, Word bgn, Word end);

/// Delete scheduling at hartid, begin-end.
void SchedDelete(Word hartid, Word bgn, Word end);
