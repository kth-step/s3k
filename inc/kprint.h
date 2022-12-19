// See LICENSE file for copyright and license details.
#pragma once
#include <stdarg.h>
#include <stddef.h>

int kprintf(const char* restrict format, ...);
int puts(const char* restrict s);
int write(const char* restrict buf, int nbytes);
int snprintf(char* str, size_t n, const char* restrict format, ...);
int vsnprintf(char* buf, size_t n, const char* restrict format, va_list args);
