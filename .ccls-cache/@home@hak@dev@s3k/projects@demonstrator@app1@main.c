#include <string.h>
char *output = (char *)0x80048800ull;

void main(void)
{
	memcpy(output, "This is application 1", 22);
}
