#include "altc/altio.h"

int alt_fgetstr(char *s, int size, ALTFILE *f)
{
	for (int i = 0; i < size; ++i) {
		s[i] = f->fgetchar(f);
		if (s[i] == '\0')
			return i;
	}
	return size;
}

int alt_getstr(char *s, int size)
{
	return alt_fgetstr(s, size, altin);
}
