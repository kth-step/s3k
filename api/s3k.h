// See LICENSE file for copyright and license details.
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "s3k_cap.g.h"
#include "s3k_consts.g.h"

static inline uint64_t _S3K_SYSCALL1(uint64_t sysnr)
{
        register uint64_t _a0 __asm__("a0") = sysnr;
        __asm__ volatile("ecall" : "+r"(_a0));
        return _a0;
}
static inline uint64_t _S3K_SYSCALL2(uint64_t sysnr, uint64_t* a1)
{
        register uint64_t _a0 __asm__("a0") = sysnr;
        register uint64_t _a1 __asm__("a1") = *a1;
        __asm__ volatile("ecall" : "+r"(_a0), "+r"(_a1));
        *a1 = _a1;
        return _a0;
}
static inline uint64_t _S3K_SYSCALL3(uint64_t sysnr, uint64_t* a1, uint64_t* a2)
{
        register uint64_t _a0 __asm__("a0") = sysnr;
        register uint64_t _a1 __asm__("a1") = *a1;
        register uint64_t _a2 __asm__("a2") = *a2;
        __asm__ volatile("ecall" : "+r"(_a0), "+r"(_a1), "+r"(_a2));
        *a1 = _a1;
        *a2 = _a2;
        return _a0;
}

static inline uint64_t _S3K_SYSCALL4(uint64_t sysnr, uint64_t* a1, uint64_t* a2, uint64_t* a3)
{
        register uint64_t _a0 __asm__("a0") = sysnr;
        register uint64_t _a1 __asm__("a1") = *a1;
        register uint64_t _a2 __asm__("a2") = *a2;
        register uint64_t _a3 __asm__("a3") = *a3;
        __asm__ volatile("ecall" : "+r"(_a0), "+r"(_a1), "+r"(_a2), "+r"(_a3));
        *a1 = _a1;
        *a2 = _a2;
        *a3 = _a3;
        return _a0;
}

static inline uint64_t _S3K_SYSCALL5(uint64_t sysnr, uint64_t* a1, uint64_t* a2, uint64_t* a3, uint64_t* a4)
{
        register uint64_t _a0 __asm__("a0") = sysnr;
        register uint64_t _a1 __asm__("a1") = *a1;
        register uint64_t _a2 __asm__("a2") = *a2;
        register uint64_t _a3 __asm__("a3") = *a3;
        register uint64_t _a4 __asm__("a4") = *a4;
        __asm__ volatile("ecall" : "+r"(_a0), "+r"(_a1), "+r"(_a2), "+r"(_a3), "+r"(_a4));
        *a1 = _a1;
        *a2 = _a2;
        *a3 = _a3;
        *a4 = _a4;
        return _a0;
}

static inline uint64_t _S3K_SYSCALL6(uint64_t sysnr, uint64_t* a1, uint64_t* a2, uint64_t* a3, uint64_t* a4,
                                     uint64_t* a5)
{
        register uint64_t _a0 __asm__("a0") = sysnr;
        register uint64_t _a1 __asm__("a1") = *a1;
        register uint64_t _a2 __asm__("a2") = *a2;
        register uint64_t _a3 __asm__("a3") = *a3;
        register uint64_t _a4 __asm__("a4") = *a4;
        register uint64_t _a5 __asm__("a5") = *a5;
        __asm__ volatile("ecall" : "+r"(_a0), "+r"(_a1), "+r"(_a2), "+r"(_a3), "+r"(_a4), "+r"(_a5));
        *a1 = _a1;
        *a2 = _a2;
        *a3 = _a3;
        *a4 = _a4;
        *a5 = _a5;
        return _a0;
}

static inline uint64_t s3k_get_pid(void)
{
        uint64_t a1;
        _S3K_SYSCALL2(S3K_SYSCALL_GET_PID, &a1);
        return a1;
}

static inline uint64_t s3k_get_reg(uint64_t reg)
{
        uint64_t a1 = reg;
        _S3K_SYSCALL2(S3K_SYSCALL_GET_REG, &a1);
        return a1;
}

static inline uint64_t s3k_set_reg(uint64_t reg, uint64_t val)
{
        return _S3K_SYSCALL3(S3K_SYSCALL_SET_REG, &reg, &val);
}

static inline uint64_t s3k_yield(void)
{
        return _S3K_SYSCALL1(S3K_SYSCALL_YIELD);
}

uint64_t s3k_read_cap(uint64_t i, cap_t* c)
{
        c->word0 = i;
        return _S3K_SYSCALL3(S3K_SYSCALL_READ_CAP, &c->word0, &c->word1);
}

uint64_t s3k_move_cap(uint64_t i, uint64_t j)
{
        return _S3K_SYSCALL3(S3K_SYSCALL_MOVE_CAP, &i, &j);
}

uint64_t s3k_delete_cap(uint64_t i)
{
        return _S3K_SYSCALL2(S3K_SYSCALL_DELETE_CAP, &i);
}

uint64_t s3k_revoke_cap(uint64_t i)
{
        return _S3K_SYSCALL2(S3K_SYSCALL_REVOKE_CAP, &i);
}

uint64_t s3k_derive_cap(uint64_t i, uint64_t j, cap_t c)
{
        return _S3K_SYSCALL5(S3K_SYSCALL_DERIVE_CAP, &i, &j, &c.word0, &c.word1);
}

uint64_t s3k_supervisor_suspend(uint64_t i, uint64_t pid)
{
        return _S3K_SYSCALL3(S3K_SYSCALL_SUP_SUSPEND, &i, &pid);
}

uint64_t s3k_supervisor_resume(uint64_t i, uint64_t pid)
{
        return _S3K_SYSCALL3(S3K_SYSCALL_SUP_RESUME, &i, &pid);
}
uint64_t s3k_supervisor_get_state(uint64_t i, uint64_t pid)
{
        if (S3K_OK == _S3K_SYSCALL3(S3K_SYSCALL_SUP_RESUME, &i, &pid))
                return i;
        return -1;
}

uint64_t s3k_supervisor_get_reg(uint64_t sup_cid, uint64_t pid, uint64_t* reg)
{
        uint64_t code = _S3K_SYSCALL4(S3K_SYSCALL_SUP_SET_REG, &sup_cid, &pid, reg);
        *reg = sup_cid;
        return code;
}

uint64_t s3k_supervisor_set_reg(uint64_t sup_cid, uint64_t pid, uint64_t reg, uint64_t val)
{
        return _S3K_SYSCALL5(S3K_SYSCALL_SUP_SET_REG, &sup_cid, &pid, &reg, &val);
}

uint64_t s3k_supervisor_read_cap(uint64_t sup_cid, uint64_t pid, uint64_t cid, cap_t* cap)
{
        cap->word0 = sup_cid;
        cap->word1 = pid;
        return _S3K_SYSCALL4(S3K_SYSCALL_SUP_SET_REG, &cap->word0, &cap->word1, &cid);
}

uint64_t s3k_supervisor_move_cap(uint64_t sup_cid, uint64_t pid, uint64_t take, uint64_t src, uint64_t dest)
{
        return _S3K_SYSCALL6(S3K_SYSCALL_SUP_MOVE_CAP, &sup_cid, &pid, &take, &src, &dest);
}
