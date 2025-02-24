#include "altc/altio.h"

int alt_fputstr(const char *s, ALTFILE *f)
{
	while (*s != '\0') {
		f->fputchar(*s, f);
		s++;
	}
	return 0;
}

int alt_putstr(const char *s)
{
	return alt_fputstr(s, altout);
}
