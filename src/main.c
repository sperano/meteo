// check https://github.com/pedgarcia/a2graph/blob/master/a2graph.c
// check https://github.com/ppelleti/json65
// 01 to 04, then  09 to 013, maybe not 12?
// http://openweathermap.org/img/w/01d.png
// http://openweathermap.org/img/w/01n.png
#include <conio.h>
#include <peekpoke.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "gfx.h"
#include "net.h"
//#include "parser.h"
#include "utils.h"

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

void handle_keyboard(MeteoConfig *config) {
    int8_t city_idx = 0;
    Units current_units = Celsius;
    CityWeather *current_city;
    while (1) {
        current_city = &config->cities[city_idx];
        update_gfx_image(current_city);
        update_gfx_text(current_city, current_units);
        switch (cgetc()) {
        case 'c':
        case 'C':
            exit_gfx();
            config_screen(config);
            init_gfx();
            set_menu_text();
            break;
        case 'q':
        case 'Q':
            return;
        case 'u':
        case 'U':
            current_units = !current_units;
            break;
        case 8:
            if (--city_idx < 0) {
                city_idx = config->nb_cities - 1;
            }
            break;
        case 21:
        case ' ':
            if (++city_idx == config->nb_cities) {
                city_idx = 0;
            }
            break;
        }
    }
}

void init(MeteoConfig *config) {
    MeteoState state = read_config(config);
    char ip_addr[IP_ADDR_STR_LENGTH];
    uint8_t i = 0;
    CityWeather *city;

    if (state != OK) {
        switch (state) {
        case ConfigOpenError:
            printf("Can't open config file \"%s\".\n", METEO_CONFIG_FILENAME);
            break;
        case ConfigInvalidMagic:
            printf("Invalid config magic number.\n");
            break;
        }
        printf("Using default configuration.\n");
        init_default_config(config);
    }
    print_config(config);
    /////////////////////
    do {
        printf("Validating Ethernet slot...\n");
        state = validate_config_ethernet(config);
        if (state == OK) {
            printf("Intializing Ethernet card...\n");
            state = init_ethernet(config);
            if (state == OK) {
                printf("Obtaining an IP address with DHCP...\n");
                state = init_dhcp(config);
                if (state == OK) {
                    get_ip_addr(ip_addr);
                    printf("IP Address: %s\n", ip_addr);
                    printf("Validating API Key...\n");
                    state = validate_config_api_key(config);
                    if (state == OK) {
                        printf("Validating Cities...\n");
                        state = validate_config_cities(config);
                        if (state == OK) {
                            for (; i < config->nb_cities; ++i) {
                                printf("\n");
                                city = &config->cities[i];
                                download_weather_data(config->api_key, city); // TODO test failure
                                print_city_weather(city);
                                city->bitmap = get_bitmap_for_icon(city->icon);
                            }
                        } else {
                            config_edit_cities(config, "No cities entered.", ESCAPE_TO_EXIT);
                            clrscr();
                        }
                    } else {
                        config_edit_api_key(config, NULL, ESCAPE_TO_EXIT);
                        clrscr();
                    }
                } else {
                    config_edit_ethernet_slot(config, ">>> Failed to get an IP Address.", 1);
                    clrscr();
                }
            } else {
                config_edit_ethernet_slot(config, ">>> Failed to initialize Ethernet.", 1);
                clrscr();
            }
        } else {
            config_edit_ethernet_slot(config, NULL, 1);
            clrscr();
        }
    } while (state != OK);
    if (config->dirty) {
        save_config(config);
    }
}

// TODO test when there is 0 in config!
int main(void) {
    MeteoConfig config;

    //POKE(_80COLON, 1);
    printf("Meteo version %s\nby Eric Sperano (2021)\n\n", METEO_VERSION);
    init(&config);
    //cgetc();
    init_gfx();
    clear_screen();
    set_menu_text();
    handle_keyboard(&config);
    exit_gfx();
    clrscr();
    free_config(&config);
    /*
    free(cw->city_name);
    free(cw->weather);
    free(cw->description);
    free(cw->icon);
    */
    return 0;
}
