#include "../inc/cap.h"
#include "../inc/consts.h"
static inline unsigned long _S3K_SYSCALL(unsigned long sysnr, unsigned long args[8], unsigned long cnt)
{
        register unsigned long t0 __asm__("t0") = sysnr;
        register unsigned long a0 __asm__("a0") = args[0];
        register unsigned long a1 __asm__("a1") = args[1];
        register unsigned long a2 __asm__("a2") = args[2];
        register unsigned long a3 __asm__("a3") = args[3];
        register unsigned long a4 __asm__("a4") = args[4];
        register unsigned long a5 __asm__("a5") = args[5];
        register unsigned long a6 __asm__("a6") = args[6];
        register unsigned long a7 __asm__("a7") = args[7];
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

static inline unsigned long s3k_get_pid(void)
{
        unsigned long args[8];
        _S3K_SYSCALL(S3K_SYSCALL_GET_PID, args, 0);
        return args[0];
}

static inline unsigned long s3k_get_reg(unsigned long reg)
{
        unsigned long args[8] = {reg};
        if (_S3K_SYSCALL(S3K_SYSCALL_GET_REG, args, 1) == 0)
                return args[0];
        return -1;
}

static inline unsigned long s3k_set_reg(unsigned long reg, unsigned long val)
{
        unsigned long args[8] = {reg, val};
        return _S3K_SYSCALL(S3K_SYSCALL_GET_REG, args, 2);
}

static inline unsigned long s3k_yield(void)
{
        unsigned long args[8] = {};
        return _S3K_SYSCALL(S3K_SYSCALL_YIELD, args, 0);
}

unsigned long s3k_read_cap(unsigned long i, cap_t* c)
{
        unsigned long args[8] = {i};
        unsigned long code = _S3K_SYSCALL(S3K_SYSCALL_READ_CAP, args, 1);
        if (!code)
                *c = (cap_t){args[0], args[1]};
        return code;
}

unsigned long s3k_move_cap(unsigned long i, unsigned long j)
{
        unsigned long args[8] = {i, j};
        return _S3K_SYSCALL(S3K_SYSCALL_MOVE_CAP, args, 2);
}

unsigned long s3k_delete_cap(unsigned long i)
{
        unsigned long args[8] = {i};
        return _S3K_SYSCALL(S3K_SYSCALL_DELETE_CAP, args, 1);
}

unsigned long s3k_revoke_cap(unsigned long i)
{
        unsigned long args[8] = {i};
        return _S3K_SYSCALL(S3K_SYSCALL_REVOKE_CAP, args, 1);
}

unsigned long s3k_derive_cap(unsigned long i, unsigned long j, cap_t c)
{
        unsigned long args[8] = {i, j, c.word0, c.word1};
        return _S3K_SYSCALL(S3K_SYSCALL_DERIVE_CAP, args, 4);
}

unsigned long s3k_supervisor_suspend(unsigned long i, unsigned long pid)
{
        unsigned long args[8] = {i, pid};
        return _S3K_SYSCALL(S3K_SYSCALL_SUP_SUSPEND, args, 2);
}

unsigned long s3k_supervisor_resume(unsigned long i, unsigned long pid)
{
        unsigned long args[8] = {i, pid};
        return _S3K_SYSCALL(S3K_SYSCALL_SUP_RESUME, args, 2);
}
unsigned long s3k_supervisor_get_state(unsigned long i, unsigned long pid)
{
        unsigned long args[8] = {i, pid};
        if (!_S3K_SYSCALL(S3K_SYSCALL_SUP_RESUME, args, 2))
                return args[0];
        return -1;
}

unsigned long s3k_supervisor_get_reg(unsigned long i, unsigned long pid, unsigned long* reg)
{
        unsigned long args[8] = {i, pid, *reg};
        unsigned long code = _S3K_SYSCALL(S3K_SYSCALL_SUP_SET_REG, args, 3);
        *reg = args[0];
        return code;
}

unsigned long s3k_supervisor_set_reg(unsigned long i, unsigned long pid, unsigned long reg, unsigned long val)
{
        unsigned long args[8] = {i, pid, reg, val};
        return _S3K_SYSCALL(S3K_SYSCALL_SUP_SET_REG, args, 4);
}

unsigned long s3k_supervisor_read_cap(unsigned long i, unsigned long pid, unsigned long cid, cap_t* cap)
{
        unsigned long args[8] = {i, pid};
        unsigned long code = _S3K_SYSCALL(S3K_SYSCALL_SUP_SET_REG, args, 2);
        if (!code) {
                cap->word0 = args[0];
                cap->word1 = args[1];
        }
        return code;
}

unsigned long s3k_supervisor_move_cap(unsigned long i, unsigned long pid, unsigned long take, unsigned long src, unsigned long dest)
{
        unsigned long args[8] = {i, pid, take, src, dest};
        return _S3K_SYSCALL(S3K_SYSCALL_SUP_MOVE_CAP, args, 5);
}
