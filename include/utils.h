#ifndef _UTILS_H
#define _UTILS_H

#include <stdio.h>
#include "types.h"

#define SCRATCH_SIZE 1024 // TODO buffer size

kelvin str_to_kelvin(const char *src);
uint16_t str_to_int(const char *src);
celsius kelvin_to_celsius(const kelvin temp);
fahrenheit celsius_to_fahrenheit(const celsius temp);
void celsius_str(char *buffer, const celsius temp);
void fail(const char *fmt, ...);

void* safe_malloc(size_t size, char *msg);
void* safe_realloc(void *ptr, size_t size, char *msg);

const char* utf8_to_ascii(const char *);

//#define print_line() printf("---------------------------------------\n")

#define KeyLeftArrow 0x08
#define KeyRightArrow 0x15
#define KeyUpArrow 0x0b
#define KeyDownArrow 0x0a
#define KeyEscape 0x1b
#endif
