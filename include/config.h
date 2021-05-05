#ifndef _METEO_CONFIG_H
#define _METEO_CONFIG_H

#define METEO_CONFIG_FILENAME "METEO.CFG"

#include <stdint.h>
#include "types.h"

void init_default_config(MeteoConfig *config);
void free_config(MeteoConfig *config);
void print_config(MeteoConfig *config);

MeteoState read_config(MeteoConfig *config);
void save_config(MeteoConfig *config);

MeteoState validate_config_ethernet(MeteoConfig *config);
MeteoState validate_config_api_key(MeteoConfig *config);
MeteoState validate_config_cities(MeteoConfig *config);

//void ensure_config(MeteoConfig *config);

void config_screen(MeteoConfig *config);

uint8_t config_edit_ethernet_slot(MeteoConfig *config, char *msg, uint8_t flag);
uint8_t config_edit_api_key(MeteoConfig *config, char *msg, uint8_t flag);
uint8_t config_edit_cities(MeteoConfig *config, char *msg, uint8_t flag);

#endif
