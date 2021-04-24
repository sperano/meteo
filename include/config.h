#ifndef _METEO_CONFIG_H
#define _METEO_CONFIG_H

#define METEO_CONFIG_FILENAME2 "METEO.CFG"

#include <stdint.h>

typedef struct MeteoConfig {
    uint8_t ethernet_slot;
    char app_id[33];
    uint8_t nb_cities;
    char **city_ids;
} MeteoConfig;

MeteoConfig* read_config();
void free_config(MeteoConfig *cfg);
void save_config(MeteoConfig *config);
void validate_config(MeteoConfig *cfg);
void print_config(MeteoConfig *cfg);
void config_screen(MeteoConfig *cfg);

#endif
