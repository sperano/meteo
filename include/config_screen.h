#ifndef _METEO_CONFIG_SCREEN_H
#define _METEO_CONFIG_SCREEN_H

#include <stdint.h>
#include "config.h"
#include "utils.h"

void config_screen(void);

ActionResult previous_menu(void *ctx, uint8_t idx);
ActionResult previous_menu_city(void *ctx, uint8_t idx);
ActionResult config_edit_ethernet_slot(void *ctx, uint8_t idx);
ActionResult config_edit_api_key(void *ctx, uint8_t idx);
ActionResult config_edit_cities(void *ctx, uint8_t idx);
ActionResult config_edit_city(void *ctx, uint8_t idx);
ActionResult config_add_city(void *ctx, uint8_t idx);

#endif
