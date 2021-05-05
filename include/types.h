#ifndef _TYPES_H
#define _TYPES_H

#include <stdint.h>

typedef enum {
    OK,
    ConfigOpenError,
    ConfigInvalidMagic,
    ConfigInvalidApiKey,
    ConfigInvalidEthernetSlot,
    ConfigInvalidNoCity,
    EthernetInitFailed,
    DHCPInitFailed,
} MeteoState;

typedef enum {Celsius, Fahrenheit} Units;

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
    char id[9];
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

typedef struct MeteoConfig {
    uint8_t ethernet_slot;
    char api_key[33];
    uint8_t nb_cities;
    CityWeather *cities;
    //char **city_ids;
    uint8_t dirty;
} MeteoConfig;

#define ESCAPE_TO_EXIT 1
typedef uint8_t (*MenuAction)(MeteoConfig *config, char *msg, uint8_t flag);

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
