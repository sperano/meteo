// check https://github.com/pedgarcia/a2graph/blob/master/a2graph.c
// check https://github.com/ppelleti/json65
// 01 to 04, then  09 to 013, maybe not 12?
// http://openweathermap.org/img/w/01d.png
// http://openweathermap.org/img/w/01n.png
#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bitmaps.h"
#include "config.h"
#include "gfx.h"
#include "parser.h"
#include "utils.h"
#ifndef MOCK_IP65
#include "ip65.h"
#endif

#define SCRATCH_SIZE 1024 // TODO buffer size
static CityWeather **cities;
static int8_t city_idx = 0;
static CityWeather *current_city;
static enum Units current_units = Celsius;

#ifdef MOCK_IP65
CityWeather* fetch_data(char *city_id) {
    static char buffer[SCRATCH_SIZE];
    uint16_t len = 0;
    char filename[15];
    FILE *f;
    CityWeather *cw;

    sprintf(filename, "W%s.JSON", city_id);
    printf(">>> Loading %s\n", filename);
    f = fopen(filename, "r");
    if (!f) {
        perror(filename);
        return NULL;
    }
    do {
        buffer[len] = fgetc(f);
        if (feof(f)) {
            break;
        }
        len++;
        if (len == SCRATCH_SIZE) {
            fail("JSON File too big");
        }
    } while(1);
    buffer[len] = 0;

    cw = safe_malloc(sizeof(CityWeather));
    cw->id = city_id;
    printf("Read %d bytes.\n", len);
    parse_api_response(cw, buffer, len);
    fclose(f);
    return cw;
}
#else
CityWeather* fetch_data(char *city_id) {
    CityWeather *cw;
    //static char url[128]; // must be 1460 bytes
    char *url = safe_malloc(1460);
    char *buffer = safe_malloc(SCRATCH_SIZE);
    uint16_t len = 0;
    char *ptr = url;

    sprintf(url, "http://api.openweathermap.org/data/2.5/weather?id=%s&appid=%s", city_id, config.app_id);

    while (*ptr) {
        *ptr = toascii(*ptr);
        ++ptr;
    }
    printf("Downloading weather for %s\n", city_id);
    len = url_download(url, buffer, SCRATCH_SIZE);
    free(url);
    printf("Downloaded %d bytes.\n", len);
    ptr = buffer;
    // go to beginning of json
    while (*ptr && *ptr != '{') {
        ++ptr; // TODO assert that we found one {}
    }
    cw = safe_malloc(sizeof(CityWeather));
    cw->id = city_id;
    //printf("json:%s\n", ptr);
    parse_api_response(cw, ptr, len-(ptr-buffer));
    free(buffer);
    return cw;
}
#endif

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


void init_cities() {
    uint8_t i;

    cities = safe_malloc(config.nb_cities * sizeof(CityWeather*));
    for (i = 0; i < config.nb_cities; ++i) {
        print_line();
        current_city = cities[i] = fetch_data(config.city_ids[i]);
        if (current_city) {
            print_city_weather(current_city);
            //cgetc();
            current_city->bitmap = get_bitmap_for_icon(current_city->icon);
            //free(current_city->icon); // we won't use it anymore
        }
    }
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
void init_internet(void) {
    unsigned char *ip_bytes = (unsigned char *)&cfg_ip;
    if (ip65_init(config.ethernet_slot)) {
        fail("Error initializing ethernet");
    }
    printf("Obtaining IP address...\n");
    if (dhcp_init()) {
        fail("Error DHCP");
    }
    printf("IP: %d.%d.%d.%d\n", ip_bytes[0], ip_bytes[1], ip_bytes[2], ip_bytes[3]);
    //printf()
}
#endif

int main(void) {
    char ch;

    printf("Meteo version %s\nby Eric Sperano (2021)\n\n", METEO_VERSION);
    read_config();
    print_config();
    validate_config();

#ifndef MOCK_IP65
    init_internet();
#endif
    init_cities();
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
