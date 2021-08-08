#ifndef _METEO_CONFIG_H
#define _METEO_CONFIG_H

#define METEO_CONFIG_FILENAME "METEO.CFG"

#define CONFIG_DEFAULT_UNITS_CELCIUS 0
#define CONFIG_DEFAULT_UNITS_FAHRENHEIT 1

#include <stdint.h>
#include "types.h"

extern MeteoConfig config;

void init_config(void);
void free_config(void);
void print_config(void);

MeteoState load_config(void);
void save_config(void);

MeteoState validate_config_ethernet(void);
MeteoState validate_config_api_key(void);
MeteoState validate_config_cities(void);

#endif
