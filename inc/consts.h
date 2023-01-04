/* See LICENSE file for copyright and license details. */
#ifndef __CONSTS_H__
#define __CONSTS_H__

#define EXCPT_NONE	    0
#define EXCPT_EMPTY	    1
#define EXCPT_INDEX	    2
#define EXCPT_COLLISION	    3
#define EXCPT_DERIVATION    4
#define EXCPT_INTERFERENCE  5
#define EXCPT_PID	    6
#define EXCPT_BUSY	    7
#define EXCPT_UNIMPLEMENTED 8

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

#endif /* __CONSTS_H__ */
