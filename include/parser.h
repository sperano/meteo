#ifndef _PARSER_H
#define _PARSER_H

#include <stdint.h>
#include <stdio.h>

typedef struct CityWeather {
    char *id;
    char *city_name;
    char *weather;
    char *description;
    char *icon;
    int16_t temperature;
    int16_t minimum;
    int16_t maximum;
    int16_t humidity;
} CityWeather;

int parse_api_response(CityWeather *cw, FILE *f);

#endif
