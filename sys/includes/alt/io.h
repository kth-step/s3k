#pragma once
#include <stdarg.h>

typedef struct file FILE;

struct file {
	int (*_putc)(struct file *, char c);
	int (*_getc)(struct file *);
	void (*_flush)(struct file *);
};

int alt_fputc(FILE *f, char c);
int alt_fgetc(FILE *f);
void alt_fflush(FILE *f);
int alt_fwrite(FILE *f, const char *buf, int size);
int alt_fread(FILE *f, char *buf, int size);
int alt_fgetstr(FILE *f, char *s);
int alt_fputstr(FILE *f, const char *s);
int alt_fputuint(FILE *f, unsigned long long val);
int alt_fputhex(FILE *f, unsigned long long val);
int alt_vfprintf(FILE *f, const char *fmt, va_list ap);
int alt_fprintf(FILE *f, const char *fmt, ...);

int alt_putc(char c);
int alt_getc(void);
void alt_flush(void);
int alt_write(const char *buf, int size);
int alt_read(char *buf, int size);
int alt_getstr(char *buf);
int alt_putstr(const char *s);
int alt_putuint(unsigned long long val);
int alt_puthex(unsigned long long val);
int alt_vprintf(const char *fmt, va_list ap);
int alt_printf(const char *fmt, ...);

int alt_sputuint(char *buf, unsigned long long val);
int alt_sputhex(char *buf, unsigned long long val);
int alt_vsprintf(char *buf, const char *fmt, va_list ap);
int alt_sprintf(char *buf, const char *fmt, ...);

int alt_snputuint(char *buf, int size, unsigned long long val);
int alt_snputhex(char *buf, int size, unsigned long long val);
int alt_vsnprintf(char *buf, int size, const char *fmt, va_list ap);
int alt_snprintf(char *buf, int size, const char *fmt, ...);
