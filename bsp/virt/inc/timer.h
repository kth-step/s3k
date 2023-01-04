#ifndef __BSP_TIMER_H__
#define __BSP_TIMER_H__
#include <stdint.h>
uint64_t timer_gettime(void);
void timer_settime(uint64_t time);
uint64_t timer_gettimer(uint64_t hartid);
void timer_settimer(uint64_t hartid, uint64_t time);
#endif /* __BSP_TIMER_H__ */
