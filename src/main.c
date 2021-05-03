// check https://github.com/pedgarcia/a2graph/blob/master/a2graph.c
// check https://github.com/ppelleti/json65
// 01 to 04, then  09 to 013, maybe not 12?
// http://openweathermap.org/img/w/01d.png
// http://openweathermap.org/img/w/01n.png
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "gfx.h"
#include "net.h"
//#include "parser.h"
#include "utils.h"

static CityWeather **cities;
static int8_t city_idx = 0;
static CityWeather *current_city;
static enum Units current_units = Celsius;

void print_city_weather(CityWeather *cw) {
    char buffer[5];
    printf("%s: %s (%s)\n", cw->city_name, cw->weather, cw->description);
    //printf("ID: %s - Icon: %s\n", cw->id, cw->icon);
    celsius_str(buffer, cw->temperatureC);
    printf("Temperature: %sC / %dF\n", buffer, cw->temperatureF);
    celsius_str(buffer, cw->minimumC);
    printf("Minimum: %sC / %dF\n", buffer, cw->minimumF);
    celsius_str(buffer, cw->maximumC);
    printf("Maximum: %sC / %dF\n", buffer, cw->maximumF);
    //printf("Humidity: %d\n", cw->humidity);
}

void next_city_index() {
    if (++city_idx == config.nb_cities) {
        city_idx = 0;
    }
    current_city = cities[city_idx];
}

void prev_city_index() {
    if (--city_idx < 0) {
        city_idx = config.nb_cities - 1;
    }
    current_city = cities[city_idx];
}

#ifndef MOCK_IP65
#endif

// TODO test when there is 0 in config!
int main(void) {
    char ch;
    FILE *file;

    printf("Meteo version %s\nby Eric Sperano (2021)\n\n", METEO_VERSION);
    read_config();
    print_config();
    validate_config();
    init_net();

    cities = safe_malloc(config.nb_cities * sizeof(CityWeather*), "Array of CityWeather*");
    for (ch = 0; ch < config.nb_cities; ++ch) {
        printf("\n");
        //print_line();
        current_city = cities[ch] = download_weather_data(config.city_ids[ch]);
        if (current_city) {
            print_city_weather(current_city);
            current_city->bitmap = get_bitmap_for_icon(current_city->icon);
            //free(current_city->icon); // we won't use it anymore
        }
    }
    //free_config(cfg);
    //cgetc();

    init_gfx();
    clear_screen();
    current_city = cities[city_idx];
    set_menu_text();
    while (1) {
        update_gfx_image(current_city);
        update_gfx_text(current_city, current_units);
        ch = cgetc();
        switch (ch) {
        case 'c':
        case 'C':
            exit_gfx();
            config_screen();
            init_gfx();
            set_menu_text();
            break;
        case 'q':
        case 'Q':
            goto exit;
        case 'u':
        case 'U':
            current_units = !current_units;
            break;
        case 8:
            prev_city_index();
            break;
        case 21:
        case ' ':
            next_city_index();
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
