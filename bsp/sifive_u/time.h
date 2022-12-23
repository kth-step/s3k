#pragma once
static inline unsigned long long read_time(void)
{
        return *MTIME;
}

static inline void write_time(unsigned long long time)
{
        *MTIME = time;
}

static inline unsigned long long read_timeout(int hartid)
{
        return *MTIMECMP(hartid);
}

static inline void write_timeout(int hartid, unsigned long long timeout)
{
        *MTIMECMP(hartid) = timeout;
}
