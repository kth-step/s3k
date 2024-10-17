#include <string.h>
char *output = (char *)0x80040800ull;

void main(void)
{
	memcpy(output, "hello, world", 13);
}
