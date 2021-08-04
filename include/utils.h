#ifndef _UTILS_H
#define _UTILS_H

#include <stdio.h>
#include "types.h"

#define BUFFER_SIZE 1024

void fail(const char *fmt, ...);
void exit_with_error(const char *fmt, ...);

void* safe_malloc(size_t size);
void* safe_realloc(void *ptr, size_t size);
char* alloc_copy(const char *src);

kelvin str_to_kelvin(const char *src);
uint16_t str_to_int(const char *src);
celsius kelvin_to_celsius(const kelvin temp);
fahrenheit celsius_to_fahrenheit(const celsius temp);
void celsius_str(char *buffer, const celsius temp);

const char* utf8_to_ascii(const char *);


#endif
