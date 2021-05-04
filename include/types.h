#ifndef _TYPES_H
#define _TYPES_H

#include <stdint.h>

enum Units{Celsius, Fahrenheit};

// kelvin has 2 "decimals"
// exemple: 273.15 kelvin is stored as 27315
typedef uint16_t kelvin;

// celsius has 1 "decimal"
// exemple: 27.5 celsius is stored as 275
typedef uint16_t celsius;

// fahrenheit has no "decimal"
typedef uint8_t fahrenheit;

//typedef uint8_t Bitmap[20][40];
typedef uint8_t **Bitmap;

typedef struct {
    char *id;
    char *city_name;
    char *weather;
    char *description;
    //union {
        char *icon;
        Bitmap bitmap;
    //};
    celsius temperatureC;
    celsius minimumC;
    celsius maximumC;
    fahrenheit temperatureF;
    fahrenheit minimumF;
    fahrenheit maximumF;
    int16_t humidity;
} CityWeather;

typedef struct {
    char *filename;
    Bitmap bitmap;
} BitmapMapping;

typedef struct {
    char *icon;
    char *filename;
} IconMapping;

typedef uint8_t (*MenuAction)(void);

typedef struct {
    char *name;
    MenuAction action;
} MenuItem;


/*
typedef struct {
    char *icon;
    Bitmap *bitmap;
} IconBitmapPair;
*/
#endif
