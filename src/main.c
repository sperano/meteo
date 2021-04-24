// check https://github.com/pedgarcia/a2graph/blob/master/a2graph.c
// check https://github.com/ppelleti/json65
// 01 to 04, then  09 to 013, maybe not 12?
// http://openweathermap.org/img/w/01d.png
// http://openweathermap.org/img/w/01n.png
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bitmaps.h"
#include "config.h"
#include "gfx.h"
#include "parser.h"
#include "utils.h"
#include "ip65.h"

CityWeather* fetch_data(char *city_id) {
    char filename[15];
    FILE *f;
    CityWeather *cw;
    int rc;

    sprintf(filename, "W%s.JSON", city_id);
    printf(">>> Loading %s\n", filename);
    f = fopen(filename, "r");
    if (!f) {
        perror(filename);
        return NULL;
    }
    cw = safe_malloc(sizeof(CityWeather));
    //strcpy(cw->id, city_id);
    cw->id = city_id;
    rc = parse_api_response(cw, f);
    fclose(f);
    if (rc == 0) {
        return cw;
    }
    free(cw);
    return NULL;
}

void print_city_weather(CityWeather *cw) {
    char buffer[5];
    printf("%s: %s (%s)\n", cw->city_name, cw->weather, cw->description);
    printf("ID: %s - Icon: %s\n", cw->id, cw->icon);
    celsius_str(buffer, cw->temperatureC);
    printf("Temperature: %sC / %dF\n", buffer, cw->temperatureF);
    celsius_str(buffer, cw->minimumC);
    printf("Minimum: %sC / %dF\n", buffer, cw->minimumF);
    celsius_str(buffer, cw->maximumC);
    printf("Maximum: %sC / %dF\n", buffer, cw->maximumF);
    //printf("Humidity: %d\n", cw->humidity);
}

//static uint8_t scratch[1024];
static CityWeather **cities;
static int8_t city_idx = 0;
static CityWeather *current_city;

void init_cities(MeteoConfig *cfg) {
    uint8_t i;

    cities = safe_malloc(cfg->nb_cities * sizeof(CityWeather*));
    for (i = 0; i < cfg->nb_cities; ++i) {
        printf("\n");
        current_city = cities[i] = fetch_data(cfg->city_ids[i]);
        if (current_city) {
            //prepare_gfx_text(current_city);
            if (!strcmp("01d", current_city->icon)) {
                current_city->bitmap = &Bitmap01d;
            } else if (!strcmp("01n", current_city->icon)) {
                current_city->bitmap = &Bitmap01n;
            } else if (!strcmp("02d", current_city->icon)) {
                current_city->bitmap = &Bitmap02d;
            } else if (!strcmp("02n", current_city->icon)) {
                current_city->bitmap = &Bitmap02n;
            } else if (!strcmp("04d", current_city->icon)) {
                current_city->bitmap = &Bitmap04d;
            } else if (!strcmp("04n", current_city->icon)) {
                current_city->bitmap = &Bitmap04d;
            } else {
                current_city->bitmap = &Bitmap404;
            }
            print_city_weather(current_city);
        }
    }
}

void next_city_index(MeteoConfig *cfg) {
    if (++city_idx == cfg->nb_cities) {
        city_idx = 0;
    }
    current_city = cities[city_idx];
}

void prev_city_index(MeteoConfig *cfg) {
    if (--city_idx < 0) {
        city_idx = cfg->nb_cities - 1;
    }
    current_city = cities[city_idx];
}

int main(void) {
    MeteoConfig *cfg;
    //CityWeather *cw;
    char ch;
    //uint8_t eth_init = ETH_INIT_DEFAULT;

    printf("Meteo version %s\nby Eric Sperano (2021)\n\n", METEO_VERSION);
    cfg = read_config();
    print_config(cfg);
    validate_config(cfg);

/*

    if (ip65_init(eth_init)) {
        fail("Error initializing ethernet");
    }
*/
    init_cities(cfg);
    //city_idx = 0;
    //free_config(cfg);
    cgetc();

    init_gfx();
    clear_screen();
    current_city = cities[city_idx];
    set_menu_text();
    while (1) {
        update_gfx_image(current_city);
        update_gfx_text(current_city);
        ch = cgetc();
        switch (ch) {
        case 'c':
        case 'C':
            exit_gfx();
            config_screen(cfg);
            init_gfx();
            set_menu_text();
            break;
        case 'q':
        case 'Q':
            goto exit;
        case 8:
            prev_city_index(cfg);
            break;
        case 21:
            next_city_index(cfg);
            break;
        }
    }

exit:
    exit_gfx();
    clrscr();
    /*
    free(cw->city_name);
    free(cw->weather);
    free(cw->description);
    free(cw->icon);
    */
    return 0;
}
