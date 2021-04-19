#ifndef _TYPES_H
#define _TYPES_H

#include <stdint.h>

// kelvin has 2 "decimals"
// exemple: 273.15 kelvin is stored as 27315
typedef uint16_t kelvin;

// celsius has 1 "decimal"
// exemple: 27.5 celsius is stored as 275
typedef uint16_t celsius;

// fahrenheit has no "decimal"
typedef uint8_t fahrenheit;

typedef struct {
    char *id;
    char *city_name;
    char *weather;
    char *description;
    char *icon;
    uint8_t (*bitmap)[20][40];
    celsius temperatureC;
    celsius minimumC;
    celsius maximumC;
    fahrenheit temperatureF;
    fahrenheit minimumF;
    fahrenheit maximumF;
    int16_t humidity;
    char text_lines[3][41];
} CityWeather;

#endif
