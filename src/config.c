#include "config.h"
#include <conio.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"

/*

Config file structure:

+----+----+
|  1 |  2 | Magic Number: E5 76
+----+----+
|  3 | Version
+----+
|  4 | Ethernet Slot
+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
|  5 |  6 |  7 |  8 |  9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 20 | 21 | 22 | 23 | 24 | 25 | 26 | 27 | 28 | 29 | 30 | 31 | 32 | 33 | 34 | 35 | 36 | AppId
+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
| 37 | Cities count
+----+----+----+----+----+----+----+----+
| 38 | 39 | 40 | 41 | 42 | 43 | 44 | 45 | CityID #1 (Always 8 bytes)
+----+----+----+----+----+----+----+----+
| 46 | 47 | 48 | 49 | 50 | 51 | 52 | 53 | CityID #2
+----+----+----+----+----+----+----+----+
 Etc...

*/

MeteoConfig* read_config() {
    static MeteoConfig config;
    uint8_t byte, i, j;
    char *appid = config.app_id;

    FILE *file = fopen(METEO_CONFIG_FILENAME2, "r");
    if (file == NULL) {
        fail("Can't open config file");
    }
    if ((byte = getc(file)) != 0xe5) { // magic number
        fail("Not a valid config file");
    }
    if ((byte = getc(file)) != 0x76) { // magic number
        fail("Not a valid config file");
    }
    getc(file); // version
    config.ethernet_slot = getc(file); // ethernet slot
    for (i = 0; i < 32; ++i) {
        config.app_id[i] = getc(file); // app id
    }
    config.app_id[32] = 0;

    config.nb_cities = getc(file);
    //printf("nb_cities=%d\n", nb_cities);
    config.city_ids = safe_malloc(config.nb_cities * sizeof(char*));
    for (i = 0; i < config.nb_cities; ++i) {
        config.city_ids[i] = safe_malloc(9 * sizeof(char));
        for (j = 0; j < 8; ++j) {
            config.city_ids[i][j] = getc(file); // city id
        }
        config.city_ids[8] = 0;
    }
    fclose(file);
    return &config;
}

void print_config(MeteoConfig* cfg) {
    uint8_t i;
    printf("AppID: %s\n", cfg->app_id);
    for (i = 0; i < cfg->nb_cities; ++i) {
        if (cfg->city_ids[i][0]) {
            printf("CityID[%d]: %s\n", i, cfg->city_ids[i]);
        }
    }
}

void validate_config(MeteoConfig* cfg) {
    uint8_t at_least_one = 0;
    uint8_t i;
    if (strlen(cfg->app_id) != 32) {
        fail("Invalid Weather App Id");
    }
    if (cfg->nb_cities == 0) {
        fail("No cities configured.");
    }
}

void config_screen(MeteoConfig *cfg) {
    clrscr();
    printf("Meteo %s Configuration\n", METEO_VERSION);
    cgetc();
}
