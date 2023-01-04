#ifndef __CAP_H__
#define __CAP_H__
/* See LICENSE file for copyright and license details. */
#include <stdbool.h>
#include <stdint.h>

#include "common.h"

enum cap_type {
        CAP_EMPTY,
        CAP_TIME,
        CAP_MEMORY,
        CAP_MONITOR,
        CAP_PMP
};

struct cap {
        uint64_t word0, word1;
};

static inline uint64_t cap_get_type(const struct cap *cap)
{
        return (cap->word0 >> 56) & 0xFFull;
}

static inline struct cap cap_time(uint64_t hartid, uint64_t begin, uint64_t free, uint64_t end)
{
        ASSERT((hartid & 0xffull) == hartid);
        ASSERT((begin & 0xffull) == begin);
        ASSERT((free & 0xffull) == free);
        ASSERT((end & 0xffull) == end);
        struct cap cap = (struct cap) {CAP_TIME, 0};
        cap.word0 |= end << 0;
        cap.word0 |= free << 8;
        cap.word0 |= begin << 16;
        cap.word0 |= hartid << 24;
        return cap;
}

static inline uint64_t cap_time_get_hartid(const struct cap *cap)
{
        ASSERT(cap_get_type(cap) == CAP_TIME);
        return (cap->word0 >> 24) & 0xffull;
}

static inline uint64_t cap_time_get_begin(const struct cap *cap)
{
        ASSERT(cap_get_type(cap) == CAP_TIME);
        return (cap->word0 >> 16) & 0xffull;
}

static inline uint64_t cap_time_get_free(const struct cap *cap)
{
        ASSERT(cap_get_type(cap) == CAP_TIME);
        return (cap->word0 >> 8) & 0xffull;
}

static inline uint64_t cap_time_get_end(const struct cap *cap)
{
        ASSERT(cap_get_type(cap) == CAP_TIME);
        return cap->word0 & 0xffull;
}

static inline void cap_time_set_hartid(struct cap *cap, uint64_t hartid)
{
        ASSERT(cap_get_type(cap) == CAP_TIME);
        ASSERT((hartid & 0xffull) == hartid);
        cap->word0 = (cap->word0 & ~0xff000000ull) | (hartid << 24);
}

static inline void cap_time_set_begin(struct cap *cap, uint64_t begin)
{
        ASSERT(cap_get_type(cap) == CAP_TIME);
        ASSERT((begin & 0xffull) == begin);
        cap->word0 = (cap->word0 & ~0xff0000ull) | (begin << 16);
}

static inline void cap_time_set_free(struct cap *cap, uint64_t free)
{
        ASSERT(cap_get_type(cap) == CAP_TIME);
        ASSERT((free & 0xffull) == free);
        cap->word0 = (cap->word0 & ~0xff00ull) | (free << 8);
}

static inline void cap_time_set_end(struct cap *cap, uint64_t end)
{
        ASSERT(cap_get_type(cap) == CAP_TIME);
        ASSERT((end & 0xffull) == end);
        cap->word0 = (cap->word0 & ~0xffull) | end;
}

static inline struct cap cap_memory(uint64_t begin, uint64_t free, uint64_t end, uint64_t rwx, uint64_t lock)
{
        ASSERT((begin & 0xffffffffull) == begin);
        ASSERT((free & 0xffffffffull) == free);
        ASSERT((end & 0xffffffffull) == end);
        ASSERT((rwx & 0x7ull) == rwx);
        ASSERT((lock & 0x1ull) == lock);
        struct cap cap = (struct cap) {CAP_MEMORY, 0};
        cap.word0 |= lock << 0;
        cap.word0 |= rwx << 1;
        cap.word0 |= begin << 4;
        cap.word1 |= end;
        cap.word1 |= free << 32;
        return cap;
}

static inline uint64_t cap_memory_get_begin(const struct cap *cap)
{
        ASSERT(cap_get_type(cap) == CAP_MEMORY);
        return (cap->word0 >> 4) & 0xffffffffull;
}

static inline uint64_t cap_memory_get_free(const struct cap *cap)
{
        ASSERT(cap_get_type(cap) == CAP_MEMORY);
        return (cap->word1 >> 32) & 0xffffffffull;
}

static inline uint64_t cap_memory_get_end(const struct cap *cap)
{
        ASSERT(cap_get_type(cap) == CAP_MEMORY);
        return cap->word1 & 0xffffffffull;
}

static inline uint64_t cap_memory_get_rwx(const struct cap *cap)
{
        ASSERT(cap_get_type(cap) == CAP_MEMORY);
        return (cap->word0 >> 1) & 0x7ull;
}

static inline uint64_t cap_memory_get_lock(const struct cap *cap)
{
        ASSERT(cap_get_type(cap) == CAP_MEMORY);
        return cap->word0 & 0x1ull;
}

static inline void cap_memory_set_begin(struct cap *cap, uint64_t begin)
{
        ASSERT(cap_get_type(cap) == CAP_MEMORY);
        ASSERT((begin & 0xffffffffull) == begin);
        cap->word0 = (cap->word0 & ~0xffffffff0ull) | (begin << 4);
}

static inline void cap_memory_set_free(struct cap *cap, uint64_t free)
{
        ASSERT(cap_get_type(cap) == CAP_MEMORY);
        ASSERT((free & 0xffffffffull) == free);
        cap->word1 = (cap->word1 & ~0xffffffff00000000ull) | (free << 32);
}

static inline void cap_memory_set_end(struct cap *cap, uint64_t end)
{
        ASSERT(cap_get_type(cap) == CAP_MEMORY);
        ASSERT((end & 0xffffffffull) == end);
        cap->word1 = (cap->word1 & ~0xffffffffull) | end;
}

static inline void cap_memory_set_rwx(struct cap *cap, uint64_t rwx)
{
        ASSERT(cap_get_type(cap) == CAP_MEMORY);
        ASSERT((rwx & 0x7ull) == rwx);
        cap->word0 = (cap->word0 & ~0xeull) | (rwx << 1);
}

static inline void cap_memory_set_lock(struct cap *cap, uint64_t lock)
{
        ASSERT(cap_get_type(cap) == CAP_MEMORY);
        ASSERT((lock & 0x1ull) == lock);
        cap->word0 = (cap->word0 & ~0x1ull) | lock;
}

static inline struct cap cap_monitor(uint64_t begin, uint64_t free, uint64_t end)
{
        ASSERT((begin & 0xffull) == begin);
        ASSERT((free & 0xffull) == free);
        ASSERT((end & 0xffull) == end);
        struct cap cap = (struct cap) {CAP_MONITOR, 0};
        cap.word0 |= end << 0;
        cap.word0 |= free << 8;
        cap.word0 |= begin << 16;
        return cap;
}

static inline uint64_t cap_monitor_get_begin(const struct cap *cap)
{
        ASSERT(cap_get_type(cap) == CAP_MONITOR);
        return (cap->word0 >> 16) & 0xffull;
}

static inline uint64_t cap_monitor_get_free(const struct cap *cap)
{
        ASSERT(cap_get_type(cap) == CAP_MONITOR);
        return (cap->word0 >> 8) & 0xffull;
}

static inline uint64_t cap_monitor_get_end(const struct cap *cap)
{
        ASSERT(cap_get_type(cap) == CAP_MONITOR);
        return cap->word0 & 0xffull;
}

static inline void cap_monitor_set_begin(struct cap *cap, uint64_t begin)
{
        ASSERT(cap_get_type(cap) == CAP_MONITOR);
        ASSERT((begin & 0xffull) == begin);
        cap->word0 = (cap->word0 & ~0xff0000ull) | (begin << 16);
}

static inline void cap_monitor_set_free(struct cap *cap, uint64_t free)
{
        ASSERT(cap_get_type(cap) == CAP_MONITOR);
        ASSERT((free & 0xffull) == free);
        cap->word0 = (cap->word0 & ~0xff00ull) | (free << 8);
}

static inline void cap_monitor_set_end(struct cap *cap, uint64_t end)
{
        ASSERT(cap_get_type(cap) == CAP_MONITOR);
        ASSERT((end & 0xffull) == end);
        cap->word0 = (cap->word0 & ~0xffull) | end;
}

static inline struct cap cap_pmp(uint64_t addr, uint64_t cfg)
{
        ASSERT((addr & 0xffffffffffffull) == addr);
        ASSERT((cfg & 0x1full) == cfg);
        struct cap cap = (struct cap) {CAP_PMP, 0};
        cap.word0 |= cfg << 0;
        cap.word0 |= addr << 5;
        return cap;
}

static inline uint64_t cap_pmp_get_addr(const struct cap *cap)
{
        ASSERT(cap_get_type(cap) == CAP_PMP);
        return (cap->word0 >> 5) & 0xffffffffffffull;
}

static inline uint64_t cap_pmp_get_cfg(const struct cap *cap)
{
        ASSERT(cap_get_type(cap) == CAP_PMP);
        return cap->word0 & 0x1full;
}

static inline void cap_pmp_set_addr(struct cap *cap, uint64_t addr)
{
        ASSERT(cap_get_type(cap) == CAP_PMP);
        ASSERT((addr & 0xffffffffffffull) == addr);
        cap->word0 = (cap->word0 & ~0x1fffffffffffe0ull) | (addr << 5);
}

static inline void cap_pmp_set_cfg(struct cap *cap, uint64_t cfg)
{
        ASSERT(cap_get_type(cap) == CAP_PMP);
        ASSERT((cfg & 0x1full) == cfg);
        cap->word0 = (cap->word0 & ~0x1full) | cfg;
}

static inline bool cap_time_is_child(const struct cap *const p, const struct cap * c)
{
        if (cap_get_type(p) == CAP_TIME && cap_get_type(c) == CAP_TIME)
                return (cap_time_get_begin(p) <= cap_time_get_begin(c)) &&
                       (cap_time_get_end(c) <= cap_time_get_end(p)) &&
                       (cap_time_get_hartid(p) == cap_time_get_hartid(c));
        return false;
}

static inline bool cap_memory_is_child(const struct cap *const p, const struct cap * c)
{
        if (cap_get_type(p) == CAP_MEMORY && cap_get_type(c) == CAP_MEMORY)
                return (cap_memory_get_begin(p) <= cap_memory_get_begin(c)) &&
                       (cap_memory_get_end(c) <= cap_memory_get_end(p));
        return false;
}

static inline bool cap_monitor_is_child(const struct cap *const p, const struct cap * c)
{
        if (cap_get_type(p) == CAP_MONITOR && cap_get_type(c) == CAP_MONITOR)
                return (cap_monitor_get_begin(p) <= cap_monitor_get_begin(c)) &&
                       (cap_monitor_get_end(c) <= cap_monitor_get_end(p));
        return false;
}

static inline bool cap_memory_can_derive(const struct cap *const p, const struct cap * c)
{
        if (cap_get_type(p) == CAP_MEMORY && cap_get_type(c) == CAP_MEMORY)
                return (cap_memory_get_lock(p) == 0) &&
                       (cap_memory_get_lock(c) == 0) &&
                       (cap_memory_get_free(p) == cap_memory_get_begin(c)) &&
                       (cap_memory_get_end(c) <= cap_memory_get_end(p)) &&
                       ((cap_memory_get_rwx(c) & cap_memory_get_rwx(p)) == cap_memory_get_rwx(c)) &&
                       (cap_memory_get_free(c) == cap_memory_get_begin(c)) &&
                       (cap_memory_get_begin(c) < cap_memory_get_end(c));
        return false;
}

static inline bool cap_time_can_derive(const struct cap *const p, const struct cap * c)
{
        if (cap_get_type(p) == CAP_TIME && cap_get_type(c) == CAP_TIME)
                return (cap_time_get_hartid(p) == cap_time_get_hartid(c)) &&
                       (cap_time_get_free(p) == cap_time_get_begin(c)) &&
                       (cap_time_get_end(c) <= cap_time_get_end(p)) &&
                       (cap_time_get_free(c) == cap_time_get_begin(c)) &&
                       (cap_time_get_begin(c) < cap_time_get_end(c));
        return false;
}

static inline bool cap_monitor_can_derive(const struct cap *const p, const struct cap * c)
{
        if (cap_get_type(p) == CAP_MONITOR && cap_get_type(c) == CAP_MONITOR)
                return (cap_monitor_get_free(p) == cap_monitor_get_begin(c)) &&
                       (cap_monitor_get_end(c) <= cap_monitor_get_end(p)) &&
                       (cap_monitor_get_free(c) == cap_monitor_get_begin(c)) &&
                       (cap_monitor_get_begin(c) < cap_monitor_get_end(c));
        return false;
}

#endif /* __CAP_H__ */
