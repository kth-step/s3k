// See LICENSE file for copyright and license details.
#pragma once

enum s3k_state {
        S3K_STATE_READY,
        S3K_STATE_RUNNING,
        S3K_STATE_WAITING,
        S3K_STATE_RECEIVING,
        S3K_STATE_SUSPENDED,
        S3K_STATE_RUNNING_THEN_SUSPEND,
        S3K_STATE_SUSPENDED_BUSY,
        S3K_STATE_RECEIVING_THEN_SUSPEND
};

enum s3k_code {
        S3K_OK,                 /* No error */
        S3K_ERROR,              /* Generic error */
        S3K_EMPTY,              /* Capability slot is empty */
        S3K_PREEMPTED,          /* Call was preempted */
        S3K_INTERRUPTED,        /* ??? */
        S3K_OCCUPIED,           /* Destination slot is occupied */
        S3K_NO_RECEIVER,        /* No receiver is waiting */
        S3K_ILLEGAL_DERIVATION, /* Capability derivation is illegal */
        S3K_SUPERVISEE_BUSY,    /* Supervisee is busy */
        S3K_INVALID_SUPERVISEE, /* Invalid supervisee PID */
        S3K_INVALID_CAPABILITY, /* Capability can not be used for this system call */
        S3K_UNIMPLEMENTED,      /* Function mussing for system call parameters */
};

enum s3k_call {
        /* Without capabilities */
        S3K_SYSCALL_GET_PID,
        S3K_SYSCALL_GET_REG,
        S3K_SYSCALL_SET_REG,
        S3K_SYSCALL_YIELD,
        /* Basic capabilities */
        S3K_SYSCALL_READ_CAP,
        S3K_SYSCALL_MOVE_CAP,
        S3K_SYSCALL_DELETE_CAP,
        S3K_SYSCALL_REVOKE_CAP,
        S3K_SYSCALL_DERIVE_CAP,
        /* Superviser capabilities functions */
        S3K_SYSCALL_SUP_SUSPEND,
        S3K_SYSCALL_SUP_RESUME,
        S3K_SYSCALL_SUP_GET_STATE,
        S3K_SYSCALL_SUP_GET_REG,
        S3K_SYSCALL_SUP_SET_REG,
        S3K_SYSCALL_SUP_READ_CAP,
        S3K_SYSCALL_SUP_MOVE_CAP,

        NUM_OF_SYSNR
};

typedef enum s3k_state s3k_state_t;
typedef enum s3k_code s3k_code_t;
typedef enum s3k_call s3k_call_t;
