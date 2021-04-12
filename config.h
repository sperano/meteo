#ifndef _METEO_CONFIG_H
#define _METEO_CONFIG_H

#define METEO_CONFIG_FILENAME "METEO.CFG"
#define MAX_CITIES 5

typedef struct MeteoConfig {
    char app_id[33];
    char city_ids[MAX_CITIES][9];
} MeteoConfig;

MeteoConfig* get_config();
void validate_config(MeteoConfig *cfg);
void print_config(MeteoConfig *cfg);

#endif
