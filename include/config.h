#ifndef _METEO_CONFIG_H
#define _METEO_CONFIG_H

#define METEO_CONFIG_FILENAME "METEO.CFG"

#define CONFIG_DEFAULT_UNITS_CELCIUS 0
#define CONFIG_DEFAULT_UNITS_FAHRENHEIT 1

#include <stdint.h>
#include "types.h"

MeteoConfig* init_config(MeteoConfig *cfg);
MeteoConfig* clone_config(MeteoConfig *dest, MeteoConfig *src);
void free_config(MeteoConfig *config, bool free_ptr);
void print_config(MeteoConfig *config);

MeteoState read_config(MeteoConfig *config);
void save_config(MeteoConfig *config);

MeteoState validate_config_ethernet(MeteoConfig *config);
MeteoState validate_config_api_key(MeteoConfig *config);
MeteoState validate_config_cities(MeteoConfig *config);

//void ensure_config(MeteoConfig *config);

#define SAVE_CONFIG 1
#define CANCEL_CONFIG 2
#define EXIT_CONFIG 3
MeteoConfig* config_screen(MeteoConfig *config);

uint8_t config_edit_ethernet_slot(void *ctx, uint8_t idx, uint8_t flags);
uint8_t config_edit_api_key(void *ctx, uint8_t idx, uint8_t flags);
uint8_t config_edit_cities(void *ctx, uint8_t idx, uint8_t flags);
uint8_t config_edit_city(void *ctx, uint8_t idx, uint8_t flags);

#endif
