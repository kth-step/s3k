// See LICENSE file for copyright and license details.
#include "sched.h"

#include <stddef.h>

#include "cap_node.h"
#include "csr.h"
#include "kprint.h"
#include "lock.h"
#include "platform.h"
#include "proc_state.h"
#include "time.h"
#include "trap.h"

static uint16_t schedule[N_QUANTUM][N_HARTS];
static lock_t lock = INIT_LOCK;

typedef struct sched_entry {
        uint8_t pid;
        uint8_t length;
} sched_entry_t;

static inline sched_entry_t sched_get(uint64_t q, uint64_t hartid);
static inline void sched_set(uint64_t q, uint64_t hartid, uint64_t pid, uint64_t length);
static inline bool sched_get_proc(uint64_t hartid, uint64_t time, proc_t** proc, uint64_t* length);

void sched_init(void)
{
        for (int i = 0; i < N_QUANTUM; i++) {
                for (int j = MIN_HARTID; j <= MAX_HARTID; j++) {
                        /* The length of the initial time slices are all N_QUANTUM and pid is 0*/
                        schedule[i][j - MIN_HARTID] = (N_QUANTUM - i) << 8;
                }
        }
}

sched_entry_t sched_get(uint64_t q, uint64_t hartid)
{
        kassert(q < N_QUANTUM);
        kassert(MIN_HARTID <= hartid && hartid <= MAX_HARTID);
        uint64_t val = schedule[q][hartid - MIN_HARTID];
        return (sched_entry_t){val & 0xFF, (val >> 8) & 0xFF};
}

void sched_set(uint64_t q, uint64_t hartid, uint64_t pid, uint64_t length)
{
        kassert(MIN_HARTID <= hartid && hartid <= MAX_HARTID);
        kassert(0 <= q && q < N_QUANTUM);
        kassert(pid == INVALID_PID || pid < N_PROC);
        kassert(length > 0 && length <= (N_QUANTUM - q));
        schedule[q][hartid - MIN_HARTID] = pid | (length << 8);
}

bool sched_get_proc(uint64_t hartid, uint64_t time, proc_t** proc, uint64_t* length)
{
        /* Calculate the current quantum */
        uint64_t quantum = time % N_QUANTUM;
        /* Get the current quantum schedule */
        synchronize();
        sched_entry_t entry = sched_get(quantum, hartid);

        /* Check if slot is invalid/inactive */
        if (entry.pid == INVALID_PID)
                return false;

        /* Check if some other thread preempts */
        for (size_t i = MIN_HARTID; i < hartid; i++) {
                if (entry.pid == sched_get(quantum, i).pid)
                        return false;
        }

        /* Set proc and length */
        *proc = &processes[entry.pid];
        *length = entry.length;
        return true;
}

void wait_and_set_timeout(uint64_t time, uint64_t length, uint64_t timeout)
{
        uint64_t start_time = time * TICKS;
        uint64_t end_time = start_time + length * TICKS - SCHEDULER_TICKS;
        uint64_t hartid = read_csr(mhartid);
        if (timeout > start_time)
                start_time = timeout;
        write_timeout(hartid, start_time);
        while (!(read_csr(mip) & 128))
                __asm__ volatile("wfi");
        write_timeout(hartid, end_time);
}

void sched_yield(void)
{
        kassert(current != NULL);
        proc_release(current);
        sched_start();
}

void sched_start(void)
{
        uintptr_t hartid = read_csr(mhartid);
        /* Process to run and number of time slices to run for */
        proc_t* proc;
        uint64_t time, length, timeout, end_time;

        while (1) {
                /* Get the current time */
                time = (read_time() / TICKS) + 1;
                /* Try getting a process at that time slice. */
                if (!sched_get_proc(hartid, time, &proc, &length))
                        continue;
                timeout = proc->regs.timeout;
                end_time = (time + length) * TICKS - SCHEDULER_TICKS;
                if (timeout >= end_time) {
                        continue;
                }
                if (proc_acquire(proc))
                        break;
        }
#ifdef MEMORY_PROTECTION
        proc_load_pmp(proc);
#endif
        wait_and_set_timeout(time, length, timeout);
        current = proc;
        trap_resume_proc();
}

void sched_update(cap_node_t* cn, uint64_t hartid, uint64_t begin, uint64_t end, uint64_t pid)
{
        kassert(begin < end);
        kassert(MIN_HARTID <= hartid && hartid <= MAX_HARTID);
        kassert(end <= N_QUANTUM);
        kassert(pid == INVALID_PID || pid < N_PROC);

        lock_acquire(&lock);
        if (!cap_node_is_deleted(cn)) {
                for (int i = begin; i < end; ++i) {
                        sched_set(i, hartid, pid, end - i);
                }
        }
        lock_release(&lock);
}
