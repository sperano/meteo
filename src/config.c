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

static MeteoConfig config;

void read_config() {
    uint8_t byte, i, j;
    char *appid = config.app_id;
    FILE *file;

    print_line();
    printf(">>> Loading config %s\n", METEO_CONFIG_FILENAME);
    file = fopen(METEO_CONFIG_FILENAME, "r");
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
    //printf("nb_cities=%d\n", config.nb_cities);
    config.city_ids = safe_malloc(config.nb_cities * sizeof(char*));
    for (i = 0; i < config.nb_cities; ++i) {
        config.city_ids[i] = safe_malloc(9 * sizeof(char));
        for (j = 0; j < 8; ++j) {
            config.city_ids[i][j] = getc(file); // city id
        }
        config.city_ids[i][8] = 0;
    }
    fclose(file);
    //return &config;
}

void free_config() {
    uint8_t i;
    for (i = 0; i < config.nb_cities; ++i) {
        free(config.city_ids[i]);
    }
    free(config.city_ids);
}

void print_config() {
    uint8_t i;
    printf("Ethernet Slot: %d\n", config.ethernet_slot);
    printf("AppID: %s\n", config.app_id);
    for (i = 0; i < config.nb_cities; ++i) {
        if (config.city_ids[i][0]) {
            printf("CityID[%d]: %s\n", i, config.city_ids[i]);
        }
    }
}

void validate_config() {
    uint8_t at_least_one = 0;
    uint8_t i;
    if (strlen(config.app_id) != 32) {
        fail("Invalid Weather App Id");
    }
    if (config.nb_cities == 0) {
        fail("No cities configured.");
    }
}

void config_screen() {
    uint8_t i;
    clrscr();
    printf("Meteo %s Configuration\n\n", METEO_VERSION);
    printf("S) Save Configuration\n\nA) Edit AppID:\n   %s\n\nC) Add City\n", config.app_id);
    for(i = 0; i < config.nb_cities; ++i) {
        printf("%d) Edit %s\n", i + 1, config.city_ids[i]);
    }
    printf("\nQ) Quit config\n");
    cgetc();
}
