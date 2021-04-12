#ifndef _METEO_H
#define _METEO_H

#define METEO_VERSION "0.1"

#pragma static-locals(on)

#include <stdint.h>
#include <stdio.h>

#define _80STORE 0xc001
#define TEXTOFF 0xc050
#define TEXTON 0xc051
#define MIXEDOFF 0xc052
#define MIXEDON 0xc053
#define PAGE2OFF 0xc054
#define PAGE2ON 0xc055
#define HIRESOFF 0xc056
#define HIRESON 0xc057

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

extern uint8_t Bitmap[20][40];

extern char TxtLine1[41];
extern unsigned char TxtLine2[];
extern unsigned char TxtLine3[];

#endif
