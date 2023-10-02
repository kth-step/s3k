#include "altio/altio.h"
#include "s3k/s3k.h"

#include <stdint.h>

int main(void)
{
	uint64_t data[4];
	char *str = (char *)data;
	while (1) {
		str[0] = 'p';
		str[1] = 'i';
		str[2] = 'n';
		str[3] = 'g';
		str[4] = '\0';
		while (s3k_sock_sendrecv(3, 0, data, 0, 0))
			;
		alt_puts(str);
	}
}
