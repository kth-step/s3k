#include <stdio.h>
#include <reent.h>

struct _reent _my_reent = _REENT_INIT(_my_reent);

static void delay(void);
int main(void)
{
    /* _impure_ptr = &_my_reent; */
    int i = 0;
    while(1) {
        printf("Hello, world!, %d\n", i++);
        /* _puts_r(&_my_reent, "Hello\n"); */
        delay();
    }
    return 0;
}

static void delay(void)
{
    for (int i = 0; i < 300000; i++) {
        asm volatile ("nop");
    }
}
