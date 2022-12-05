// See LICENSE file for copyright and license details.
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "s3k_cap.g.h"
#include "s3k_consts.g.h"

static inline uint64_t _S3K_SYSCALL(uint64_t sysnr, uint64_t args[8], uint64_t cnt)
{
        register uint64_t t0 __asm__("t0") = sysnr;
        register uint64_t a0 __asm__("a0") = args[0];
        register uint64_t a1 __asm__("a1") = args[1];
        register uint64_t a2 __asm__("a2") = args[2];
        register uint64_t a3 __asm__("a3") = args[3];
        register uint64_t a4 __asm__("a4") = args[4];
        register uint64_t a5 __asm__("a5") = args[5];
        register uint64_t a6 __asm__("a6") = args[6];
        register uint64_t a7 __asm__("a7") = args[7];
        switch (cnt) {
        case 0:
                __asm__ volatile("ecall"
                                 : "+r"(t0), "=r"(a0), "=r"(a1), "=r"(a2), "=r"(a3), "=r"(a4), "=r"(a5), "=r"(a6),
                                   "=r"(a7));
                break;
        case 1:
                __asm__ volatile("ecall"
                                 : "+r"(t0), "+r"(a0), "=r"(a1), "=r"(a2), "=r"(a3), "=r"(a4), "=r"(a5), "=r"(a6),
                                   "=r"(a7));
                break;
        case 2:
                __asm__ volatile("ecall"
                                 : "+r"(t0), "+r"(a0), "+r"(a1), "=r"(a2), "=r"(a3), "=r"(a4), "=r"(a5), "=r"(a6),
                                   "=r"(a7));
                break;
        case 3:
                __asm__ volatile("ecall"
                                 : "+r"(t0), "+r"(a0), "+r"(a1), "+r"(a2), "=r"(a3), "=r"(a4), "=r"(a5), "=r"(a6),
                                   "=r"(a7));
                break;
        case 4:
                __asm__ volatile("ecall"
                                 : "+r"(t0), "+r"(a0), "+r"(a1), "+r"(a2), "+r"(a3), "=r"(a4), "=r"(a5), "=r"(a6),
                                   "=r"(a7));
                break;
        case 5:
                __asm__ volatile("ecall"
                                 : "+r"(t0), "+r"(a0), "+r"(a1), "+r"(a2), "+r"(a3), "+r"(a4), "=r"(a5), "=r"(a6),
                                   "=r"(a7));
                break;
        case 6:
                __asm__ volatile("ecall"
                                 : "+r"(t0), "+r"(a0), "+r"(a1), "+r"(a2), "+r"(a3), "+r"(a4), "+r"(a5), "=r"(a6),
                                   "=r"(a7));
                break;
        case 7:
                __asm__ volatile("ecall"
                                 : "+r"(t0), "+r"(a0), "+r"(a1), "+r"(a2), "+r"(a3), "+r"(a4), "+r"(a5), "+r"(a6),
                                   "=r"(a7));
                break;
        case 8:
                __asm__ volatile("ecall"
                                 : "+r"(t0), "+r"(a0), "+r"(a1), "+r"(a2), "+r"(a3), "+r"(a4), "+r"(a5), "+r"(a6),
                                   "+r"(a7));
                break;
        }
        args[0] = a0;
        args[1] = a1;
        args[2] = a2;
        args[3] = a3;
        args[4] = a4;
        args[5] = a5;
        args[6] = a6;
        args[7] = a7;
        return t0;
}

static inline uint64_t s3k_get_pid(void)
{
        uint64_t args[8];
        _S3K_SYSCALL(S3K_SYSCALL_GET_PID, args, 0);
        return args[0];
}

static inline uint64_t s3k_get_reg(uint64_t reg)
{
        uint64_t args[8] = {reg};
        if (_S3K_SYSCALL(S3K_SYSCALL_GET_REG, args, 1) == 0)
                return args[0];
        return -1;
}

static inline uint64_t s3k_set_reg(uint64_t reg, uint64_t val)
{
        uint64_t args[8] = {reg, val};
        return _S3K_SYSCALL(S3K_SYSCALL_GET_REG, args, 2);
}

static inline uint64_t s3k_yield(void)
{
        uint64_t args[8] = {};
        return _S3K_SYSCALL(S3K_SYSCALL_YIELD, args, 0);
}

uint64_t s3k_read_cap(uint64_t i, cap_t* c)
{
        uint64_t args[8] = {i};
        uint64_t code = _S3K_SYSCALL(S3K_SYSCALL_READ_CAP, args, 1);
        if (!code)
                *c = (cap_t){args[0], args[1]};
        return code;
}

uint64_t s3k_move_cap(uint64_t i, uint64_t j)
{
        uint64_t args[8] = {i, j};
        return _S3K_SYSCALL(S3K_SYSCALL_MOVE_CAP, args, 2);
}

uint64_t s3k_delete_cap(uint64_t i)
{
        uint64_t args[8] = {i};
        return _S3K_SYSCALL(S3K_SYSCALL_DELETE_CAP, args, 1);
}

uint64_t s3k_revoke_cap(uint64_t i)
{
        uint64_t args[8] = {i};
        return _S3K_SYSCALL(S3K_SYSCALL_REVOKE_CAP, args, 1);
}

uint64_t s3k_derive_cap(uint64_t i, uint64_t j, cap_t c)
{
        uint64_t args[8] = {i, j, c.word0, c.word1};
        return _S3K_SYSCALL(S3K_SYSCALL_DERIVE_CAP, args, 4);
}

uint64_t s3k_supervisor_suspend(uint64_t i, uint64_t pid)
{
        uint64_t args[8] = {i, pid};
        return _S3K_SYSCALL(S3K_SYSCALL_SUP_SUSPEND, args, 2);
}

uint64_t s3k_supervisor_resume(uint64_t i, uint64_t pid)
{
        uint64_t args[8] = {i, pid};
        return _S3K_SYSCALL(S3K_SYSCALL_SUP_RESUME, args, 2);
}
uint64_t s3k_supervisor_get_state(uint64_t i, uint64_t pid)
{
        uint64_t args[8] = {i, pid};
        if (!_S3K_SYSCALL(S3K_SYSCALL_SUP_RESUME, args, 2))
                return args[0];
        return -1;
}

uint64_t s3k_supervisor_get_reg(uint64_t i, uint64_t pid, uint64_t* reg)
{
        uint64_t args[8] = {i, pid, *reg};
        uint64_t code = _S3K_SYSCALL(S3K_SYSCALL_SUP_SET_REG, args, 3);
        *reg = args[0];
        return code;
}

uint64_t s3k_supervisor_set_reg(uint64_t i, uint64_t pid, uint64_t reg, uint64_t val)
{
        uint64_t args[8] = {i, pid, reg, val};
        return _S3K_SYSCALL(S3K_SYSCALL_SUP_SET_REG, args, 4);
}

uint64_t s3k_supervisor_read_cap(uint64_t i, uint64_t pid, uint64_t cid, cap_t* cap)
{
        uint64_t args[8] = {i, pid};
        uint64_t code = _S3K_SYSCALL(S3K_SYSCALL_SUP_SET_REG, args, 2);
        if (!code) {
                cap->word0 = args[0];
                cap->word1 = args[1];
        }
        return code;
}

uint64_t s3k_supervisor_move_cap(uint64_t i, uint64_t pid, uint64_t take, uint64_t src, uint64_t dest)
{
        uint64_t args[8] = {i, pid, take, src, dest};
        return _S3K_SYSCALL(S3K_SYSCALL_SUP_MOVE_CAP, args, 5);
}
