/* freestanding.c - tiny freestanding mem* routines.
 *
 * The kernel links with -nostdlib -lgcc. At -O2 GCC inlines the small
 * struct copies, but at -Os (the ch32v307 size-constrained build) it emits
 * calls to memcpy/memset, which then must be provided by the kernel itself.
 */

#include <stddef.h>
#include <stdint.h>

__attribute__((used)) void *memcpy(void *dst, const void *src, size_t n)
{
	unsigned char *d = dst;
	const unsigned char *s = src;
	while (n--)
		*d++ = *s++;
	return dst;
}

__attribute__((used)) void *memset(void *dst, int c, size_t n)
{
	unsigned char *d = dst;
	while (n--)
		*d++ = (unsigned char)c;
	return dst;
}
