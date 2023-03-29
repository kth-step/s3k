/**
 * @file consts.h
 * @brief Constants and enums.
 * @copyright MIT License
 * @author Henrik Karlsson (henrik10@kth.se)
 */

#ifndef __CONSTS_H__
#define __CONSTS_H__

/** Process state flags
 * PSF_BUSY: Some core
 * PSF_BLOCK: Waiting for IPC.
 * PSF_SUSPEND: Waiting for monitor
 */
#define PSF_BUSY 1
#define PSF_BLOCK 2
#define PSF_SUSPEND 4
#define PSF_WAITING 8

/** Process states
 *
 */
#define PS_READY 0
#define PS_RUNNING 1
#define PS_BLOCKED 2
#define PS_BLOCKED_BUSY 3
#define PS_SUSPENDED 4
#define PS_SUSPENDED_BUSY 5

#endif /* __CONSTS_H__ */
