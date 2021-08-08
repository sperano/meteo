#ifndef _TYPES_H
#define _TYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    FailOpenConfigFileWrite = 1,
    FailOpenBitmapFileRead,
    FailBitmapElementCount,
    FailJSONParsing,
    FailMalloc,
    FailRealloc,
    FailEthernetConfigCancelled,
    FailEthernetInit,
    FailDHCPInit,
    FailAPIKeyConfigCancelled,
    FailDownloadData,
    FailNewCityCancelled,
    FailJSONTooLarge,
} FailCode;

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

#define CITY_ID_LEN 8
typedef struct {
    char id[CITY_ID_LEN + 1];
    char *name;
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

#define API_KEY_LEN 32
typedef struct MeteoConfig {
    uint8_t ethernet_slot;
    char api_key[API_KEY_LEN + 1];
    Units default_units;
    uint8_t nb_cities;
    CityWeather **cities;
    bool dirty;
} MeteoConfig;

typedef enum  {
    ExitConfig,
    SaveAndExitConfig,
    CancelAndExitConfig,
    EthernetSlotConfigured,
    EditEthernetSlotCancelled,
    APIKeyConfigured,
    EditAPIKeyCancelled,
    CityIDConfigured,
    EditCityIDCancelled,
    CityDeleted,
    CityAdded,
    UnitConfigured,
    CityAddFailed,
    CityAddCancelled,
    PreviousMenu,
    PreviousMenuCity,
    DataFetched,
    FetchDataFailed,
} ActionResult;

#define ACCEPT_ESCAPE 1
#define ACCEPT_HEXA 2
#define ACCEPT_NUMBER 4
#define ACCEPT_SPACE 8

typedef ActionResult (*MenuAction)(void *ctx, uint8_t idx);

typedef bool (*MenuVisibilityCheck)(void *ctx);

typedef void (*MenuInit)(void *ctx);

typedef struct {
    char *name;
    MenuAction action;
    MenuVisibilityCheck visibility_check;
} MenuItem;

/*
typedef struct {
    MeteoConfig *config;
    CityWeather *city;
} Context;
*/

#endif
