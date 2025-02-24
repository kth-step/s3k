#include "altc/altio.h"

int alt_fputchar(int c, ALTFILE *f)
{
	return f->fputchar(c, f);
}

int alt_putchar(int c)
{
	return alt_fputchar(c, altout);
}
