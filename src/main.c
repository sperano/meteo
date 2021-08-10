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
// TODO 50d (smoke)
#ifdef WITH_CONFIG
void _config_ethernet(void) {
    if (config_edit_ethernet_slot(&config, 0) == EditEthernetSlotCancelled) {
        fail(FailEthernetConfigCancelled, 0);
    };
    clrscr();
}

void _config_other_ethernet_slot(char *msg) {
    printf("Failed to %s.\n\nPress any key to select another slot.\n", msg);
    cgetc();
    _config_ethernet();
}
#endif

void print_city_weather(CityWeather *cw) {
    char buffer[5];
    //printf("%s: %s (%s)\n", cw->name, cw->weather, cw->description);
    printf("ID: %s - Icon: %s\n", cw->id, cw->icon);
    celsius_str(buffer, cw->temperatureC);
    printf("Temperature: %sC / %dF\n", buffer, cw->temperatureF);
    celsius_str(buffer, cw->minimumC);
    printf("Minimum: %sC / %dF\n", buffer, cw->minimumF);
    celsius_str(buffer, cw->maximumC);
    printf("Maximum: %sC / %dF\n", buffer, cw->maximumF);
    //printf("Humidity: %d\n", cw->humidity);
}

void handle_keyboard() {
    static char downloading_msg[] = {
        0x84, 0xef, 0xf7, 0xee, 0xec, 0xef, 0xe1, 0xe4, 0xe9, 0xee,
        0xe7, 0xa0, 0xf7, 0xe5, 0xe1, 0xf4, 0xe8, 0xe5, 0xf2, 0xa0,
        0xe4, 0xe1, 0xf4, 0xe1, 0xae, 0xae, 0xae, 0xa0, 0xa0, 0xa0,
        0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0,
    };
    int8_t city_idx = 0;
    Units current_units = config.default_units;
    CityWeather *current_city;
    while (true) {
        current_city = config.cities[city_idx];
        update_gfx_image(current_city);
        update_gfx_text(current_city, current_units);
        switch (cgetc()) {
#ifdef WITH_CONFIG
        case 'c':
        case 'C':
            exit_gfx();
            config_screen();
            current_units = config.default_units;
            city_idx = 0;
            init_gfx();
            set_menu_text();
            break;
#endif
        case 'q':
        case 'Q':
            return;
        case 'r':
        case 'R':
            clear_text();
            set_text_line(downloading_msg, 21);
            download_weather_data(current_city);
            set_menu_text();
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
    //print_config();
    /////////////////////
    do {
        printf("Validating Ethernet slot...\n");
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
                    printf("Validating API key...\n");
                    state = validate_config_api_key();
                    if (state == OK) {
                        printf("Validating cities...\n");
                        state = validate_config_cities();
                        if (state == OK) {
                            for (; i < config.nb_cities; ++i) {
                                printf("\n");
                                city = config.cities[i];
                                if (download_weather_data_w(city) < 1) {
                                    fail(FailDownloadData, i);
                                }
                                print_city_weather(city);
                                city->bitmap = get_bitmap_for_icon(city->icon);
                            }
                        } else {
                            #ifdef WITH_CONFIG
                            if (config_add_city(&config, 0) == CityAddCancelled) {
                                fail(FailNewCityCancelled, 0);
                            }
                            clrscr();
                            #endif
                        }
                    } else {
                        #ifdef WITH_CONFIG
                        if (config_edit_api_key(&config, 0) == EditAPIKeyCancelled) {
                            fail(FailAPIKeyConfigCancelled, 0);
                        };
                        clrscr();
                        #endif
                    }
                } else {
                    #ifdef WITH_CONFIG
                    _config_other_ethernet_slot("obtain an IP address");
                    #endif
                }
            } else {
                #ifdef WITH_CONFIG
                _config_other_ethernet_slot("initialize ethernet");
                #endif
            }
        } else {
            #ifdef WITH_CONFIG
            _config_ethernet();
            #endif
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
