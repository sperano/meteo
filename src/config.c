#include <conio.h>
#include <ctype.h>
#include <peekpoke.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ip65.h"
#include "config.h"
#include "gfx.h"
#include "net.h"
#include "utils.h"

#pragma static-locals(on)

// TODO limit to add city?

/*

Config file structure:

+----+----+
|  1 |  2 | Magic Number: E5 76
+----+----+
|  3 | Version
+----+
|  4 | Ethernet Slot
+----+
|  5 | Default Units
+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
|  6 |  7 |  8 |  9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 20 | 21 | 22 | 23 | 24 | 25 | 26 | 27 | 28 | 29 | 30 | 31 | 32 | 33 | 34 | 35 | 36 | 37 | AppId
+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
| 38 | Cities count
+----+----+----+----+----+----+----+----+
| 39 | 40 | 41 | 42 | 43 | 44 | 45 | 46 | CityID #1 (Always 8 bytes)
+----+----+----+----+----+----+----+----+
| 47 | 48 | 49 | 50 | 51 | 52 | 53 | 54 | CityID #2
+----+----+----+----+----+----+----+----+
 Etc...

*/

/**
 *
 */
MeteoConfig* init_config(MeteoConfig *config) {
    uint8_t i = 0;
    if (config == NULL) {
        config = safe_malloc(sizeof(MeteoConfig));
    }
    for (; i < 32; ++i) {
        config->api_key[i] = '.';
    }
    config->api_key[32] = 0;
    config->ethernet_slot = ETH_INIT_DEFAULT;
    config->default_units = Fahrenheit;
    config->nb_cities = 0;
    config->cities = NULL;
    config->dirty = false;
    return config;
}

MeteoConfig* clone_config(MeteoConfig *dest, MeteoConfig *src) {
    uint8_t i = 0;
    CityWeather *cw_dest, *cw_src;

    if (dest == NULL) {
        dest = safe_malloc(sizeof(MeteoConfig));
    }
    dest->ethernet_slot = src->ethernet_slot;
    dest->default_units = src->default_units;
    dest->dirty = src->dirty;
    strcpy(dest->api_key, src->api_key);
    dest->nb_cities = src->nb_cities;
    dest->cities = safe_malloc(dest->nb_cities * sizeof(CityWeather*));
    for (; i < dest->nb_cities; ++i) {
        dest->cities[i] = safe_malloc(sizeof(CityWeather));
        cw_dest = dest->cities[i];
        cw_src = src->cities[i];

        strcpy(cw_dest->id, cw_src->id);
        cw_dest->name = alloc_copy(cw_src->name);
        cw_dest->description = alloc_copy(cw_src->description);
        cw_dest->icon = alloc_copy(cw_src->icon);
        cw_dest->weather = alloc_copy(cw_src->weather);
        cw_dest->temperatureC = cw_src->temperatureC;
        cw_dest->minimumC = cw_src->minimumC;
        cw_dest->maximumC = cw_src->maximumC;
        cw_dest->temperatureF = cw_src->temperatureF;
        cw_dest->minimumF = cw_src->minimumF;
        cw_dest->maximumF = cw_src->maximumF;
        cw_dest->humidity = cw_src->humidity;
        cw_dest->bitmap = cw_src->bitmap;
    }
    return dest;
}

/**
 * Free the memory that is dynamically allocated in the MeteoConfig structure.
 */
void free_config(MeteoConfig *config, bool free_ptr) {
    uint8_t i = 0;
    for (; i < config->nb_cities; ++i) {
        free(config->cities[i]->name);
        free(config->cities[i]->weather);
        free(config->cities[i]->description);
        free(config->cities[i]->icon);
        free(config->cities[i]);
    }
    free(config->cities);
    if (free_ptr) {
        free(config);
    }
}

/**
 * Print the MeteoConfig structure content to standard output.
 */
void print_config(MeteoConfig *config) {
    uint8_t i;
    printf("Ethernet Slot: %d\nKey: \"%s\"\nNb Cities: %d\n", config->ethernet_slot, config->api_key, config->nb_cities);
    for (i = 0; i < config->nb_cities; ++i) {
        printf("CityID[%d]: %s\n", i, config->cities[i]->id);
    }
}

/**
 *
 */
void save_config(MeteoConfig *config) {
    uint8_t i, j;
    FILE *file = fopen(METEO_CONFIG_FILENAME, "w");
    if (file == NULL) {
        fail("Can't open config file\n");
    }
    putc(0xe5, file);
    putc(0x76, file);
    putc(0x01, file); // version
    putc(config->ethernet_slot, file);
    putc(config->default_units == Celsius ?
        CONFIG_DEFAULT_UNITS_CELCIUS : CONFIG_DEFAULT_UNITS_FAHRENHEIT, file);
    for (i = 0; i < 32; ++i) {
        putc(config->api_key[i], file);
    }
    putc(config->nb_cities, file);
    for (i = 0; i < config->nb_cities; ++i) {
        for (j = 0; j < 8; ++j) {
            putc(config->cities[i]->id[j], file);
        }
    }
    fclose(file);
    config->dirty = 0;
}

// TODO rename to load_config ???
MeteoState read_config(MeteoConfig *config) {
    uint8_t i, j;
    FILE *file;
    CityWeather *cw;

    printf("Loading config %s\n", METEO_CONFIG_FILENAME);
    file = fopen(METEO_CONFIG_FILENAME, "r");
    if (file == NULL) {
        return ConfigOpenError;
    }
    if (getc(file) != 0xe5) { // magic number
        fclose(file);
        return ConfigInvalidMagic;
    }
    if (getc(file) != 0x76) { // magic number
        fclose(file);
        return ConfigInvalidMagic;
    }
    getc(file); // version
    config->ethernet_slot = getc(file); // ethernet slot
    switch (getc(file)) {
    case CONFIG_DEFAULT_UNITS_CELCIUS:
        config->default_units = Celsius;
        break;
    case CONFIG_DEFAULT_UNITS_FAHRENHEIT:
        config->default_units = Fahrenheit;
        break;
    }
    for (i = 0; i < 32; ++i) {
        config->api_key[i] = getc(file); // app id
    }
    config->api_key[32] = 0;
    config->nb_cities = getc(file);
    if (config->nb_cities > 0) {
        config->cities = safe_malloc(config->nb_cities * sizeof(CityWeather*));
    }
    for (i = 0; i < config->nb_cities; ++i) {
        cw = config->cities[i] = safe_malloc(sizeof(CityWeather));
        //config->city_ids[i] = safe_malloc(9 * sizeof(char), "CityID");
        for (j = 0; j < 8; ++j) {
            cw->id[j] = getc(file); // city id
        }
        cw->id[8] = 0;
    }
    fclose(file);
    config->dirty = 0;
    return OK;
}

/**
 *
 */
MeteoState validate_config_ethernet(MeteoConfig *config) {
    return (config->ethernet_slot < 1 || config->ethernet_slot > 7) ? ConfigInvalidEthernetSlot : OK;
}

MeteoState validate_config_api_key(MeteoConfig *config) {
    uint8_t i = 0;
    char ch;
    if (strlen(config->api_key) != 32) {
        return ConfigInvalidApiKey;
    }
    for (; i < 32; ++i) {
        ch = config->api_key[i];
        if (!((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'))) {
            return ConfigInvalidApiKey;
        }
    }
    return OK;
}

MeteoState validate_config_cities(MeteoConfig *config) {
    return config->nb_cities ? OK : ConfigInvalidNoCity;
}
