#include "altc/altio.h"

int alt_fgetchar(ALTFILE *f)
{
	return f->fgetchar(f);
}

int alt_getchar(void)
{
	return alt_fgetchar(altin);
}
