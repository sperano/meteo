#include <conio.h>
#include <ctype.h>
#include <peekpoke.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ip65.h"
#include "config.h"
#include "gfx.h"
#include "utils.h"

/*

Config file structure:

+----+----+
|  1 |  2 | Magic Number: E5 76
+----+----+
|  3 | Version
+----+
|  4 | Ethernet Slot
+----+
|  5 | Default Units
+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
|  6 |  7 |  8 |  9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 20 | 21 | 22 | 23 | 24 | 25 | 26 | 27 | 28 | 29 | 30 | 31 | 32 | 33 | 34 | 35 | 36 | 37 | AppId
+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
| 38 | Cities count
+----+----+----+----+----+----+----+----+
| 39 | 40 | 41 | 42 | 43 | 44 | 45 | 46 | CityID #1 (Always 8 bytes)
+----+----+----+----+----+----+----+----+
| 47 | 48 | 49 | 50 | 51 | 52 | 53 | 54 | CityID #2
+----+----+----+----+----+----+----+----+
 Etc...

*/

#define print_config_header() printf("Meteo %s - Configuration\n----------------------------------------", METEO_VERSION)

/**
 *
 */
void init_default_config(MeteoConfig *config) {
    uint8_t i = 0;
    for (; i < 32; ++i) {
        config->api_key[i] = '.';
    }
    config->api_key[32] = 0;
    config->ethernet_slot = ETH_INIT_DEFAULT;
    config->default_units = Fahrenheit;
    config->nb_cities = 0;
    config->dirty = 1;
}

/**
 * Free the memory that is dynamically allocated in the MeteoConfig structure.
 */
void free_config(MeteoConfig *config) {
    uint8_t i = 0;
    for (; i < config->nb_cities; ++i) {
        free(config->cities[i].city_name);
        free(config->cities[i].weather);
        free(config->cities[i].description);
        free(config->cities[i].icon);
    }
    free(config->cities);
}

/**
 * Print the MeteoConfig structure content to standard output.
 */
void print_config(MeteoConfig *config) {
    uint8_t i;
    printf("Ethernet Slot: %d\nKey: \"%s\"\nNb Cities: %d\n", config->ethernet_slot, config->api_key, config->nb_cities);
    for (i = 0; i < config->nb_cities; ++i) {
        printf("CityID[%d]: %s\n", i, config->cities[i].id);
    }
}

/**
 *
 */
void save_config(MeteoConfig *config) {
    uint8_t i, j;
    FILE *file = fopen(METEO_CONFIG_FILENAME, "w");
    if (file == NULL) {
        fail("Can't open config file\n");
    }
    putc(0xe5, file);
    putc(0x76, file);
    putc(0x01, file); // version
    putc(config->ethernet_slot, file);
    putc(config->default_units == Celsius ?
        CONFIG_DEFAULT_UNITS_CELCIUS : CONFIG_DEFAULT_UNITS_FAHRENHEIT, file);
    for (i = 0; i < 32; ++i) {
        putc(config->api_key[i], file);
    }
    putc(config->nb_cities, file);
    for (i = 0; i < config->nb_cities; ++i) {
        for (j = 0; j < 8; ++j) {
            putc(config->cities[i].id[j], file);
        }
    }
    fclose(file);
    config->dirty = 0;
}


MeteoState read_config(MeteoConfig *config) {
    uint8_t i, j;
    //char *api_key = config.api_key;
    FILE *file;

    printf("Loading config %s\n", METEO_CONFIG_FILENAME);
    file = fopen(METEO_CONFIG_FILENAME, "r");
    if (file == NULL) {
        return ConfigOpenError;
    }
    if (getc(file) != 0xe5) { // magic number
        fclose(file);
        return ConfigInvalidMagic;
    }
    if (getc(file) != 0x76) { // magic number
        fclose(file);
        return ConfigInvalidMagic;
    }
    getc(file); // version
    config->ethernet_slot = getc(file); // ethernet slot
    switch (getc(file)) {
    case CONFIG_DEFAULT_UNITS_CELCIUS:
        config->default_units = Celsius;
        break;
    case CONFIG_DEFAULT_UNITS_FAHRENHEIT:
        config->default_units = Fahrenheit;
        break;
    }
    for (i = 0; i < 32; ++i) {
        config->api_key[i] = getc(file); // app id
    }
    config->api_key[32] = 0;
    config->nb_cities = getc(file);
    if (config->nb_cities > 0) {
        config->cities = safe_malloc(config->nb_cities * sizeof(CityWeather), "Array of CityWeather");
    }
    for (i = 0; i < config->nb_cities; ++i) {
        //config->city_ids[i] = safe_malloc(9 * sizeof(char), "CityID");
        for (j = 0; j < 8; ++j) {
            config->cities[i].id[j] = getc(file); // city id
        }
        config->cities[i].id[8] = 0;
    }
    fclose(file);
    config->dirty = 0;
    return OK;
}

/**
 *
 */
MeteoState validate_config_ethernet(MeteoConfig *config) {
    return (config->ethernet_slot < 1 || config->ethernet_slot > 7) ? ConfigInvalidEthernetSlot : OK;
}

MeteoState validate_config_api_key(MeteoConfig *config) {
    uint8_t i = 0;
    char ch;
    if (strlen(config->api_key) != 32) {
        return ConfigInvalidApiKey;
    }
    for (; i < 32; ++i) {
        ch = config->api_key[i];
        if (!((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'))) {
            return ConfigInvalidApiKey;
        }
    }
    return OK;
}

MeteoState validate_config_cities(MeteoConfig *config) {
    return config->nb_cities ? OK : ConfigInvalidNoCity;
}


uint8_t config_edit_ethernet_slot2(MeteoConfig *config, char *msg) {
    return config_edit_ethernet_slot(config, msg, 0);
}

/**
 *
 */
uint8_t config_edit_ethernet_slot(MeteoConfig *config, char *msg, uint8_t escape) {
    char ch;
    uint8_t current_value = config->ethernet_slot;
    void *ptr = (void*)(VideoBases[7]+16);
    char data[1];

    clrscr();
    print_config_header();
    printf("\nThe ethernet card is usually\ninstalled in slot #%d.\n\n\n"
           "Ethernet Slot: #\n\n"
           "Enter a number between 1 and 7\nor use the arrow keys.\n\n"
           "\n\n\n\n%s\n\n\n\n\n\n", ETH_INIT_DEFAULT, msg == NULL ? "" : msg);
    if (escape) {
        printf("Press [Return] to continue.\nPress [Esc] to exit.");
    } else {
        printf("\nPress [Return] to continue.");
    }

    while (1) {
        data[0] = '0' + current_value;
        memcpy(ptr, data , 1);
        ch = cgetc();
        if (ch >= '1' && ch <= '7') {
            current_value = ch - '0';
        } else if (ch == '\r') {
            break;
        } else {
            switch(ch) {
            case KeyLeftArrow:
            case KeyUpArrow:
                current_value = current_value == 1 ? 7 : current_value - 1;
                break;
            case KeyRightArrow:
            case KeyDownArrow:
                current_value = current_value == 7 ? 1 : current_value + 1;
                break;
            case KeyEscape:
                if (escape) {
                    exit(1);
                }
                break;
            }
        }
    }
    config->ethernet_slot = current_value;
    config->dirty = 1;
    return 0;
}

/**
 *
 */
uint8_t config_edit_api_key(MeteoConfig *config, char *msg, uint8_t flag) {
    char ch;
    char id[32];
    char *idptr = config->api_key;
    uint8_t stop = 0;
    uint8_t pos = 0;
    void *ptr = (void*)(VideoBases[13]+4);

    memcpy(id, config->api_key, 32);
    for (ch = 0; ch < 32; ++ch) {
        if (id[ch] >= 'A' && id[ch] <= 'F') {
            id[ch] -= 0x40;
        }
    }
    clrscr();
    print_config_header();
    printf("\nGenerate an API Key at:\nhttps://openweathermap.org\n\n"
           "The API Key is a 32 characters long\nhexadecimal string.\n\n\n\n"
           "    API Key:\n\n\n\n\n%s\n\n\n\n", msg == NULL ? "" : msg);
    if (flag & ESCAPE_TO_EXIT) {
        printf("Use Arrow keys to move cursor.\n\nPress [Return] to continue.\nPress [Esc] to exit.");
    } else {
        printf("\nUse Arrow keys to move cursor.\n\nPress [Return] to continue.");
    }
    while (!stop) {
        id[pos] = id[pos] += 0x80;
        //data[0] = '0' + current_value;
        memcpy(ptr, id, 32);
        ch = cgetc();

        if (ch >= '0' && ch <= '9') {
            id[pos] = ch;
            pos = pos == 31 ? 31 : pos + 1;
        } else if ((ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F')) {
            id[pos] = toupper(ch) - 0x40;
            pos = pos == 31 ? 31 : pos + 1;
        } else {
            switch (ch) {
            case '\r':
                id[pos] -= 0x80;
                stop = 1;
                break;
            case KeyLeftArrow:
                id[pos] -= 0x80;
                if (pos) {
                    pos--;
                }
                break;
            case KeyRightArrow:
                id[pos] -= 0x80;
                if (pos < 31) {
                    pos++;
                }
                break;
            case KeyEscape:
                if (flag & ESCAPE_TO_EXIT) {
                    exit(1);
                }
                break;
            }
        }
    }
    for (ch = 0; ch < 32; ++ch) {
        idptr[ch] = id[ch];
        if (idptr[ch] >= ('A'-0x40) && idptr[ch] <= ('F'-0x40)) {
            idptr[ch] += 0x40;
        }
    }
    config->dirty = 1;
    return 0;
}

/**
 *
 */
uint8_t config_edit_cities(MeteoConfig *config, char *msg, uint8_t flag) {
    MenuItem *menu = safe_malloc((config->nb_cities + 4) * sizeof(MenuItem), "City MenuItem");
    uint8_t i = 0;
    uint8_t nb_cities = config->nb_cities;
    uint8_t selected = 0;
    char ch;
    uint8_t exit = 0;

    // TODO fixed width of menu items like main config menu
    for(; i < nb_cities; ++i) {
        menu[i].name = config->cities[i].city_name;
    }
    menu[nb_cities].name = "-";
    menu[nb_cities + 1].name = "Add a city";
    menu[nb_cities + 2].name = "Previous menu";

    clrscr();
    print_config_header();
        //printf("\nFind the ID for your city in this file:\n"
        //       "http://bulk.openweathermap.org/sample/city.list.json.gz\n"
        //       "\n\n\n\n\n%s", msg == NULL ? "" : msg);
    while (!exit) {
        draw_menu(4, selected, nb_cities + 3, menu);
        ch = cgetc();
        switch (ch) {
        case KeyLeftArrow:
        case KeyUpArrow:
            do {
                selected = selected ? selected - 1 : nb_cities + 2;
            } while(menu[selected].name[0] == '-');
            break;
        case KeyRightArrow:
        case KeyDownArrow:
        case '\t':
            do {
                selected = selected == nb_cities + 2 ? 0 : selected + 1;
            } while(menu[selected].name[0] == '-');
            break;
        case ' ':
        case '\r':
            if (selected >= 0 && selected <= nb_cities) {
                config_edit_city(config, selected, NULL, 0);
                clrscr();
                print_config_header(); // probably refresh everything!
            } else if (selected == nb_cities + 1) {
                // add

            } else if (selected == nb_cities + 2) {
                // previous menu
                exit = 1;
            }
            break;
        }
    }
    //cgetc();
    free(menu);
    return 0;
}

#define CONFIG_EDIT_CITY_MENU_SIZE 4
uint8_t config_edit_city(MeteoConfig *config, uint8_t city_idx, char *msg, uint8_t flag) {
    static MenuItem menu[] = {
        {"Edit ID                ", NULL},
        {"Delete city            ", NULL},
        {"Fetch data             ", NULL},
        {"Previous Menu          ", NULL},
    };
    CityWeather *cw = &config->cities[city_idx];
    uint8_t exit = 0;
    uint8_t selected = 0;

    clrscr();
    print_config_header();
    printf("\n %s (%s)\n", cw->city_name, cw->id);
    if (config->default_units == Celsius) {
        printf(" Temp: C\n");
    } else {
        printf(" Temp: F\n");
    }
    while (!exit) {
        draw_menu(8, selected, CONFIG_EDIT_CITY_MENU_SIZE, menu);

        cgetc();
        exit = 1;
    }
    /*
    while (!exit) {
        draw_menu(4, selected, nb_cities + 3, menu);
        ch = cgetc();
        switch (ch) {
        case KeyLeftArrow:
        case KeyUpArrow:
            do {
                selected = selected ? selected - 1 : nb_cities + 2;
            } while(menu[selected].name[0] == '-');
            break;
        case KeyRightArrow:
        case KeyDownArrow:
        case '\t':
            do {
                selected = selected == nb_cities + 2 ? 0 : selected + 1;
            } while(menu[selected].name[0] == '-');
            break;
        case ' ':
        case '\r':
            if (selected >= 0 && selected <= nb_cities) {

            } else if (selected == nb_cities + 1) {
                // add

            } else if (selected == nb_cities + 2) {
                // previous menu
                exit = 1;
            }
            break;
        }
    }
    free(menu);
    */
    return 0;
}

uint8_t config_set_celcius(MeteoConfig *config, char *msg, uint8_t flag) {
    config->default_units = Celsius;
    return 1;
}

uint8_t config_set_fahrenheit(MeteoConfig *config, char *msg, uint8_t flag) {
    config->default_units = Fahrenheit;
    return 1;
}

uint8_t config_edit_default_units(MeteoConfig *config, char *msg, uint8_t flag) {
    static MenuItem menu[] = {
        {"Celcius              ", config_set_celcius},
        {"Fahrenheit           ", config_set_fahrenheit},
    };
    uint8_t exit = 0;
    uint8_t selected = config->default_units == Celsius ? 0 : 1;

    clrscr();
    print_config_header();
    while (!exit) {
        draw_menu(4, selected, 2, menu);
        switch (cgetc()) {
        case KeyLeftArrow:
        case KeyUpArrow:
        case KeyRightArrow:
        case KeyDownArrow:
        case '\t':
            selected = !selected;
            break;
        case ' ':
        case '\r':
            exit = menu[selected].action(config, NULL, 0);
            break;
        }
    }
    return 0;
}

uint8_t config_do_quit(MeteoConfig *config, char *msg, uint8_t flag) {
    //save_config(config);
    return 1;
}

uint8_t config_do_cancel(MeteoConfig *config, char *msg, uint8_t flag) {
    //free_config(config);
    //read_config(config);
    return 1;
}

#define TOTAL_MENU_ITEMS 8 // TODO variable? sizeof...
void config_screen(MeteoConfig *config) {
    static MenuItem config_menu[] = {
        {"Ethernet Slot              ", config_edit_ethernet_slot},
        {"API Key                    ", config_edit_api_key},
        {"Cities                     ", config_edit_cities},
        {"Default Units              ", config_edit_default_units},
        {"-"},
        {"-"},
        {"Save and exit configuration", config_do_quit},
        {"Cancel                     ", config_do_cancel},
    };
    uint8_t selected = 0;
    char ch;
    uint8_t exit = 0;

    clrscr();
    print_config_header();
    while (!exit) {
        //sprintf(config_menu[0].name, "Ethernet Slot: %d", config.ethernet_slot);
        //sprintf(config_menu[1].name, "AppID: %s", config.api_key);
        //sprintf(config_menu[2].name, "Cities (%d)", config.nb_cities);
        draw_menu(4, selected, TOTAL_MENU_ITEMS, config_menu);
        ch = cgetc();
        //printf("%x ", ch);
        switch (ch) {
        case KeyLeftArrow:
        case KeyUpArrow:
            do {
                selected = selected ? selected - 1 : TOTAL_MENU_ITEMS - 1;
            } while(config_menu[selected].name[0] == '-');
            break;
        case KeyRightArrow:
        case KeyDownArrow:
        case '\t':
            do {
                selected = selected == TOTAL_MENU_ITEMS - 1 ? 0 : selected + 1;
            } while(config_menu[selected].name[0] == '-');
            break;
        case ' ':
        case '\r':
            exit = config_menu[selected].action(config, NULL, 0);
            clrscr();
            print_config_header();
            break;
        }
    }
}
