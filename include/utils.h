#ifndef _UTILS_H
#define _UTILS_H

#include <stdio.h>
#include "types.h"

#define BUFFER_SIZE 1024

uint8_t my_asm_func(uint8_t x, uint8_t y);

void fail(FailCode fail_code, uint16_t extra);

void* safe_malloc(size_t size);
void* safe_realloc(void *ptr, size_t size);
char* alloc_copy(const char *src);

kelvin str_to_kelvin(const char *src);
uint16_t str_to_int(const char *src);
celsius kelvin_to_celsius(const kelvin temp);
fahrenheit celsius_to_fahrenheit(const celsius temp);
void celsius_str(char *buffer, const celsius temp);

const char* utf8_to_ascii(const char *);

int16_t download_weather_data_w(CityWeather *city_weather);

#endif
