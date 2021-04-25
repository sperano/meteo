#ifndef _UTILS_H
#define _UTILS_H

#include "types.h"

kelvin str_to_kelvin(const char *src);
uint16_t str_to_int(const char *src);
celsius kelvin_to_celsius(const kelvin temp);
fahrenheit celsius_to_fahrenheit(const celsius temp);
void celsius_str(char *buffer, const celsius temp);
void fail(char *msg);
void* safe_malloc(size_t size);

#endif
