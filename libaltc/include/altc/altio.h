#pragma once
#include <stdarg.h>
#include <stddef.h>

typedef struct altfile ALTFILE;

struct altfile {
	int (*fputchar)(int c, ALTFILE *);
	int (*fgetchar)(ALTFILE *);
};

extern ALTFILE *const altin;
extern ALTFILE *const altout;

// void alt_finit(ALTFILE *f);

int alt_fputchar(int c, ALTFILE *f);
int alt_fputstr(const char *s, ALTFILE *f);
int alt_fwrite(const char *s, int size, ALTFILE *f);
int alt_fgetchar(ALTFILE *f);
int alt_fgetstr(char *s, int size, ALTFILE *f);
int alt_fread(char *s, int size, ALTFILE *f);

int alt_putstr(const char *s);
int alt_putchar(int c);
int alt_write(const char *s, int size);
int alt_getchar(void);
int alt_getstr(char *s, int size);
int alt_read(char *s, int size);

int alt_printf(const char *format, ...);
int alt_vprintf(const char *format, va_list ap);
int alt_sprintf(char *s, const char *format, ...);
int alt_vsprintf(char *s, const char *format, va_list ap);
int alt_snprintf(char *s, size_t n, const char *format, ...);
int alt_vsnprintf(char *s, size_t n, const char *format, va_list ap);
int alt_fprintf(ALTFILE *f, const char *format, ...);
int alt_vfprintf(ALTFILE *f, const char *format, va_list ap);
