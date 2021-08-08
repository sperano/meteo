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
#include "config_screen.h"
#include "gfx.h"
#include "net.h"
#include "ui.h"
#include "utils.h"

// TODO humidity
// TODO 50d, 50n (mist)
void _config_ethernet(void) {
    if (config_edit_ethernet_slot(&config, 0) == EditEthernetSlotCancelled) {
        exit_with_error("Configuration of ethernet slot cancelled.\n");
    };
    clrscr();
}

void print_city_weather(CityWeather *cw) {
    char buffer[5];
    printf("%s: %s (%s)\n", cw->name, cw->weather, cw->description);
    //printf("ID: %s - Icon: %s\n", cw->id, cw->icon);
    celsius_str(buffer, cw->temperatureC);
    printf("Temperature: %sC / %dF\n", buffer, cw->temperatureF);
    celsius_str(buffer, cw->minimumC);
    printf("Minimum: %sC / %dF\n", buffer, cw->minimumF);
    celsius_str(buffer, cw->maximumC);
    printf("Maximum: %sC / %dF\n", buffer, cw->maximumF);
    //printf("Humidity: %d\n", cw->humidity);
}

void handle_keyboard() {
    int8_t city_idx = 0;
    Units current_units = config.default_units;
    CityWeather *current_city;
    while (true) {
        current_city = config.cities[city_idx];
        update_gfx_image(current_city);
        update_gfx_text(current_city, current_units);
        switch (cgetc()) {
        case 'c':
        case 'C':
            exit_gfx();
            config_screen();
            current_units = config.default_units;
            city_idx = 0;
            init_gfx();
            set_menu_text();
            break;
        case 'q':
        case 'Q':
            return;
        case 'r':
        case 'R':
            //return;
            break;
        case 'u':
        case 'U':
            current_units = !current_units;
            break;
        case KeyLeftArrow:
            if (--city_idx < 0) {
                city_idx = config.nb_cities - 1;
            }
            break;
        case KeyRightArrow:
        case ' ':
        case '\t':
            if (++city_idx == config.nb_cities) {
                city_idx = 0;
            }
            break;
        }
    }
}

void init() {
    //MeteoConfig *config = init_config(NULL);
    MeteoState state;
    uint8_t i = 0;
    char ip_addr[IP_ADDR_STR_LENGTH];
    CityWeather *city;

    init_config();
    state = load_config();

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
        init_config();
    }
    print_config();
    /////////////////////
    do {
        //printf("Validating Ethernet slot...\n");
        state = validate_config_ethernet();
        if (state == OK) {
            printf("Intializing Ethernet card...\n");
            state = init_ethernet();
            if (state == OK) {
                printf("Obtaining an IP address with DHCP...\n");
                state = init_dhcp();
                if (state == OK) {
                    get_ip_addr(ip_addr);
                    printf("IP Address: %s\n", ip_addr);
                    //printf("Validating API Key...\n");
                    state = validate_config_api_key();
                    if (state == OK) {
                        printf("Validating Cities...\n");
                        state = validate_config_cities();
                        if (state == OK) {
                            for (; i < config.nb_cities; ++i) {
                                printf("\n");
                                city = config.cities[i];
                                //printf("City: %s\n", city->id);
                                if (!download_weather_data(city)) {
                                    exit_with_error("Failed to download data for city: %s\nUsing API Key: %s\n", city->id, config.api_key);
                                }
                                print_city_weather(city);
                                city->bitmap = get_bitmap_for_icon(city->icon);
                            }
                        } else {
                            if (config_add_city(&config, 0) == CityAddCancelled) {
                                exit_with_error("Configuration of a new city cancelled.\n");
                            }
                            clrscr();
                        }
                    } else {
                        if (config_edit_api_key(&config, 0) == EditAPIKeyCancelled) {
                            exit_with_error("Configuration of API key cancelled.\n");
                        };
                        clrscr();
                    }
                } else {
                    printf("Failed to obtain an IP address.\n\nPress any key to select another slot.\n");
                    cgetc();
                    _config_ethernet();
                }
            } else {
                printf("Failed to initialize ethernet.\n\nPress any key to select another slot.\n");
                cgetc();
                _config_ethernet();
            }
        } else {
            _config_ethernet();
        }
    } while (state != OK);
    if (config.dirty) {
        save_config();
    }
}

int main(void) {
    _heapadd ((void *) 0x0800, 0x1800);
    printf("Meteo version %s\nby Eric Sperano (2021)\n\n", METEO_VERSION);
    init();
    //POKE(_80COLON, 1);

    init_gfx();
    clear_screen();
    set_menu_text();
    handle_keyboard();
    exit_gfx();
    clrscr();
    free_config();
    return 0;
}
