#ifndef _METEO_CONFIG_H
#define _METEO_CONFIG_H

#define METEO_CONFIG_FILENAME "METEO.CFG"

#include <stdint.h>

typedef struct MeteoConfig {
    uint8_t ethernet_slot;
    char app_id[33];
    uint8_t nb_cities;
    char **city_ids;
} MeteoConfig;

void read_config();
void free_config();
void save_config();
void validate_config();
void print_config();
void config_screen();

extern MeteoConfig config;

#endif
