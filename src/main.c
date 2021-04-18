// check https://github.com/pedgarcia/a2graph/blob/master/a2graph.c
// check https://github.com/ppelleti/json65
// 01 to 04, then  09 to 013
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

CityWeather* fetch_data(char *city_id) {
    char filename[15];
    FILE *f;
    CityWeather *cw;
    int rc;

    sprintf(filename, "W%s.JSON", city_id);
    //printf(">>> Loading %s\n", filename);
    f = fopen(filename, "r");
    if (!f) {
        perror(filename);
        return NULL;
    }
    cw = malloc(sizeof(CityWeather));
    if (cw == NULL) {
        printf("Not enough memory for CityWeather\n");
        exit(20);
    }
    cw->id = city_id;
    rc = parse_api_response(cw, f);
    fclose(f);
    if (rc == 0) {
        return cw;
    }
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
static CityWeather *cities[MAX_CITIES];
static int city_idx = 0;

void init_cities(MeteoConfig *cfg) {
    uint8_t i;
    CityWeather *cw;
    for (i = 0; i < MAX_CITIES; ++i) {
        if (cfg->city_ids[i][0]) {
            printf("\n");
            cw = cities[i] = fetch_data(cfg->city_ids[i]);
            if (cw) {
                prepare_gfx_text(cw);
                if (!strcmp("01d", cw->icon)) {
                    cw->bitmap = &Bitmap01d;
                } else if (!strcmp("01n", cw->icon)) {
                    cw->bitmap = &Bitmap01n;
                } else if (!strcmp("02n", cw->icon)) {
                    cw->bitmap = &Bitmap02n;
                } else {
                    cw->bitmap = &Bitmap404;
                }
                print_city_weather(cw);
            }
        }
    }
}

void init_city_index() {
    city_idx = 0;
    while (!cities[city_idx]) {
        city_idx++;
    }
}

void next_city_index() {
    ++city_idx;
    for (; city_idx < MAX_CITIES; ++city_idx) {
        if (cities[city_idx]) {
            return;
        }
    }
    for (city_idx = 0; city_idx < MAX_CITIES; ++city_idx) {
        if (cities[city_idx]) {
            return;
        }
    }
}

void prev_city_index() {
    for (--city_idx; city_idx >= 0; --city_idx) {
        if (cities[city_idx]) {
            return;
        }
    }
    for (city_idx = MAX_CITIES - 1; city_idx >= 0; --city_idx) {
        if (cities[city_idx]) {
            return;
        }
    }
}

int main(void) {
    MeteoConfig *cfg;
    char ch;

    printf("Meteo version %s\nby Eric Sperano (2021)\n\n", METEO_VERSION);
    cfg = get_config();
    print_config(cfg);
    validate_config(cfg);
    init_cities(cfg);
    init_city_index();
    cgetc();

    init_gfx();
    clear_screen();

    update_gfx_image(cities[city_idx]);
    update_gfx_text(cities[city_idx]);
    while (1) {
        ch = cgetc();
        switch (ch) {
        case 'q':
        case 'Q':
            goto exit;
        case 8:
            prev_city_index();
            update_gfx_image(cities[city_idx]);
            update_gfx_text(cities[city_idx]);
            break;
        case 21:
            next_city_index();
            update_gfx_image(cities[city_idx]);
            update_gfx_text(cities[city_idx]);
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
