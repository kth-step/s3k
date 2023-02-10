/* See LICENSE file for copyright and license details. */
#pragma once

typedef enum reg_id {
	/* General purpose registers */
	REG_PC,	  ///< Program counter
	REG_RA,	  ///< Return address (GPR)
	REG_SP,	  ///< Stack pointer (GPR)
	REG_GP,	  ///< Global pointer (GPR)
	REG_TP,	  ///< Thread pointer (GPR)
	REG_T0,	  ///< Temporary register (GPR)
	REG_T1,	  ///< Temporary register (GPR)
	REG_T2,	  ///< Temporary register (GPR)
	REG_S0,	  ///< Saved register/Stack frame pointer (GPR)
	REG_S1,	  ///< Saved register (GPR)
	REG_A0,	  ///< Argument/Return register (GPR)
	REG_A1,	  ///< Argument/Return register (GPR)
	REG_A2,	  ///< Argument register (GPR)
	REG_A3,	  ///< Argument register (GPR)
	REG_A4,	  ///< Argument register (GPR)
	REG_A5,	  ///< Argument register (GPR)
	REG_A6,	  ///< Argument register (GPR)
	REG_A7,	  ///< Argument register (GPR)
	REG_S2,	  ///< Saved register (GPR)
	REG_S3,	  ///< Saved register (GPR)
	REG_S4,	  ///< Saved register (GPR)
	REG_S5,	  ///< Saved register (GPR)
	REG_S6,	  ///< Saved register (GPR)
	REG_S7,	  ///< Saved register (GPR)
	REG_S8,	  ///< Saved register (GPR)
	REG_S9,	  ///< Saved register (GPR)
	REG_S10,  ///< Saved register (GPR)
	REG_S11,  ///< Saved register (GPR)
	REG_T3,	  ///< Temporary register (GPR)
	REG_T4,	  ///< Temporary register (GPR)
	REG_T5,	  ///< Temporary register (GPR)
	REG_T6,	  ///< Temporary register (GPR)
	/* Virtual registers */
	REG_CAUSE,  ///< Exception cause code.
	REG_TVAL,   ///< Exception value.
	REG_EPC,    ///< Exception program counter.
	REG_TVEC,   ///< Exception handling vector.
	REG_PMP,    ///< PMP configuration.
	REG_COUNT   ///< *Number of S3K registers.*
} RegId;

/** Process state flags
 * PSF_BUSY: Some core
 * PSF_BLOCK: Waiting for IPC.
 * PSF_SUSPEND: Waiting for monitor
 */
#define PSF_BUSY    1
#define PSF_BLOCK   2
#define PSF_SUSPEND 4

/** Process states
 *
 */
#define PS_READY	  0
#define PS_RUNNING	  1
#define PS_BLOCKED	  2
#define PS_BLOCKED_BUSY	  3
#define PS_SUSPENDED	  4
#define PS_SUSPENDED_BUSY 5
