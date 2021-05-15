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

#pragma static-locals(on)

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
MeteoConfig* init_config(MeteoConfig *config) {
    uint8_t i = 0;
    if (config == NULL) {
        config = safe_malloc(sizeof(MeteoConfig));
    }
    for (; i < 32; ++i) {
        config->api_key[i] = '.';
    }
    config->api_key[32] = 0;
    config->ethernet_slot = ETH_INIT_DEFAULT;
    config->default_units = Fahrenheit;
    config->nb_cities = 0;
    config->cities = NULL;
    config->dirty = false;
    return config;
}

MeteoConfig* clone_config(MeteoConfig *dest, MeteoConfig *src) {
    uint8_t i = 0;
    CityWeather *cw_dest, *cw_src;

    if (dest == NULL) {
        dest = safe_malloc(sizeof(MeteoConfig));
    }
    dest->ethernet_slot = src->ethernet_slot;
    dest->default_units = src->default_units;
    dest->dirty = src->dirty;
    strcpy(dest->api_key, src->api_key);
    dest->nb_cities = src->nb_cities;
    dest->cities = safe_malloc(dest->nb_cities * sizeof(CityWeather*));
    for (; i < dest->nb_cities; ++i) {
        dest->cities[i] = safe_malloc(sizeof(CityWeather));
        cw_dest = dest->cities[i];
        cw_src = src->cities[i];

        strcpy(cw_dest->id, cw_src->id);
        cw_dest->name = alloc_copy(cw_src->name);
        cw_dest->description = alloc_copy(cw_src->description);
        cw_dest->icon = alloc_copy(cw_src->icon);
        cw_dest->weather = alloc_copy(cw_src->weather);
        cw_dest->temperatureC = cw_src->temperatureC;
        cw_dest->minimumC = cw_src->minimumC;
        cw_dest->maximumC = cw_src->maximumC;
        cw_dest->temperatureF = cw_src->temperatureF;
        cw_dest->minimumF = cw_src->minimumF;
        cw_dest->maximumF = cw_src->maximumF;
        cw_dest->humidity = cw_src->humidity;
        cw_dest->bitmap = cw_src->bitmap;
    }
    return dest;
}

/**
 * Free the memory that is dynamically allocated in the MeteoConfig structure.
 */
void free_config(MeteoConfig *config, bool free_ptr) {
    uint8_t i = 0;
    for (; i < config->nb_cities; ++i) {
        free(config->cities[i]->name);
        free(config->cities[i]->weather);
        free(config->cities[i]->description);
        free(config->cities[i]->icon);
        free(config->cities[i]);
    }
    free(config->cities);
    if (free_ptr) {
        free(config);
    }
}

/**
 * Print the MeteoConfig structure content to standard output.
 */
void print_config(MeteoConfig *config) {
    uint8_t i;
    printf("Ethernet Slot: %d\nKey: \"%s\"\nNb Cities: %d\n", config->ethernet_slot, config->api_key, config->nb_cities);
    for (i = 0; i < config->nb_cities; ++i) {
        printf("CityID[%d]: %s\n", i, config->cities[i]->id);
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
            putc(config->cities[i]->id[j], file);
        }
    }
    fclose(file);
    config->dirty = 0;
}


MeteoState read_config(MeteoConfig *config) {
    uint8_t i, j;
    FILE *file;
    CityWeather *cw;

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
        config->cities = safe_malloc(config->nb_cities * sizeof(CityWeather*));
    }
    for (i = 0; i < config->nb_cities; ++i) {
        cw = config->cities[i] = safe_malloc(sizeof(CityWeather));
        //config->city_ids[i] = safe_malloc(9 * sizeof(char), "CityID");
        for (j = 0; j < 8; ++j) {
            cw->id[j] = getc(file); // city id
        }
        cw->id[8] = 0;
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

#pragma warn (unused-param, push, off)
void config_menu_init(Menu *menu, void *ctx) {
#ifndef NOCONSOLE
    clrscr();
#endif
    print_config_header();
}
#pragma warn (unused-param, pop)

#pragma warn (unused-param, push, off)
void config_menu_init2(Menu *menu, void *ctx) {
    Context *ctx_ = (Context*)ctx;
    CityWeather *cw = ctx_->city;
    char buffer[16];
#ifndef NOCONSOLE
    clrscr();
#endif
    print_config_header();
    printf("\n  %s (%s)\n  %s\n\n", cw->name, cw->id, cw->description);
    celsius_str(buffer, cw->temperatureC);
    printf("  Temperature: %4sC    %dF\n", buffer, cw->temperatureF);
    celsius_str(buffer, cw->minimumC);
    printf("  Minimum:     %4sC    %dF\n", buffer, cw->minimumF);
    celsius_str(buffer, cw->maximumC);
    printf("  Maximum:     %4sC    %dF\n", buffer, cw->maximumF);
}
#pragma warn (unused-param, pop)

#pragma warn (unused-param, push, off)
void config_menu_init3(Menu *menu, void *ctx) {
#ifndef NOCONSOLE
    clrscr();
#endif
    print_config_header();
    printf("\n  Are you sure you want to delete\n  %s ?", ((Context*)ctx)->city->name);
}
#pragma warn (unused-param, pop)

/**
 *
 */
#pragma warn (unused-param, push, off)
uint8_t config_edit_ethernet_slot(void *ctx, uint8_t idx, uint8_t flags) {
#ifndef NOCONSOLE
    char ch;
    uint8_t current_value = ((MeteoConfig*)ctx)->ethernet_slot;
    uint8_t orig_value = current_value;
    void *ptr = (void*)(VideoBases[7]+16);
    char data[1];

    clrscr();
    print_config_header();
    printf("\nThe ethernet card is usually\ninstalled in slot #%d.\n\n\n"
           "Ethernet Slot: #\n\n"
           "Enter a number between 1 and 7\nor use the arrow keys.\n\n"
           "\n\n\n\n\n\n\n\n\n\n", ETH_INIT_DEFAULT);
    if (flags & ESCAPE_TO_EXIT) {
        printf("Press [Return] to continue.\nPress [Esc] to exit.");
    } else {
        printf("\nPress [Return] to continue.");
    }
    while (true) {
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
                if (flags & ESCAPE_TO_EXIT) {
                    exit(1);
                }
                break;
            }
        }
    }
    ((MeteoConfig*)ctx)->ethernet_slot = current_value;
    ((MeteoConfig*)ctx)->dirty = orig_value != current_value;
#endif
    return 0;
}
#pragma warn (unused-param, pop)

/**
 *
 */
#pragma warn (unused-param, push, off)
uint8_t config_edit_api_key(void *ctx, uint8_t idx, uint8_t flags) {
#ifndef NOCONSOLE
    MeteoConfig *cfg = (MeteoConfig*)ctx;
    char dest[API_KEY_LEN + 1];

    strcpy(dest, cfg->api_key);
    clrscr();
    print_config_header();
    printf("\nGenerate an API Key at:\nhttps://openweathermap.org\n\n"
           "The API Key is a 32 characters long\nhexadecimal string.\n\n\n\n"
           "    API Key:\n\n\n\n\n\n\n\n\n");
    if (flags & ESCAPE_TO_EXIT) {
        printf("Use Arrow keys to move cursor.\n\nPress [Return] to continue.\nPress [Esc] to exit.");
    } else {
        printf("\nUse Arrow keys to move cursor.\n\nPress [Return] to continue.");
    }
    text_input(4, 13, API_KEY_LEN, dest, cfg->api_key, ACCEPT_HEXA);
    if (strcmp(dest, cfg->api_key)) {
        strcpy(cfg->api_key, dest);
        cfg->dirty = true;
    }
#endif
    return 0;
}
#pragma warn (unused-param, pop)

/**
 *
 */
#pragma warn (unused-param, push, off)
uint8_t config_edit_cities(void *ctx, uint8_t idx, uint8_t flags) {
    MeteoConfig *config = (MeteoConfig*)ctx;
    uint8_t nb_cities = 0;
    uint8_t i = 0;
    MenuItem *menu_items;
    Menu menu = {
        "",
        4,  // y
        1, // interlines
        0, // selected
        DEFAULT_MENU_LEFT_PAD,
        0, // total_items
        NULL,
        config_menu_init
    };
    while (true) {
        nb_cities = config->nb_cities;
        menu_items = safe_malloc((nb_cities + 3) * sizeof(MenuItem));
        menu.items = menu_items;
        menu.total_items = nb_cities + 3;
        for(i = 0; i < nb_cities; ++i) {
            menu_items[i].name = config->cities[i]->name;
            menu_items[i].action = config_edit_city;
            menu_items[i].visibility_check = NULL;
        }
        menu_items[nb_cities].name = "-";
        menu_items[nb_cities].visibility_check = NULL;
        menu_items[nb_cities + 1].name = "Add a city";
        menu_items[nb_cities + 1].visibility_check = NULL;
        menu_items[nb_cities + 2].name = "Previous menu";
        menu_items[nb_cities + 2].action = previous_menu;
        menu_items[nb_cities + 2].visibility_check = NULL;
        if (do_menu(&menu, ctx) == EXIT_CONFIG) {
            break;
        };
        free(menu_items);
    }
    return 0;
}
#pragma warn (unused-param, pop)

#pragma warn (unused-param, push, off)
uint8_t config_edit_city_id(void *ctx, uint8_t idx, uint8_t flags) {
#ifndef NOCONSOLE
    Context *ctx_ = (Context*)ctx;
    char dest[CITY_ID_LEN + 1];
    strcpy(dest, ctx_->city->id);
    clrscr();
    print_config_header();
    printf("\n  City: %s\n\n  Edit the City ID:\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n", ctx_->city->name);
    if (flags & ESCAPE_TO_EXIT) {
        printf("Use Arrow keys to move cursor.\n\nPress [Return] to continue.\nPress [Esc] to exit.");
    } else {
        printf("\nUse Arrow keys to move cursor.\n\nPress [Return] to continue.");
    }
    text_input(21, 5, CITY_ID_LEN, dest, ctx_->city->id, ACCEPT_NUMBER | ACCEPT_SPACE);
    clrscr();
    print_config_header();
    if (strcmp(dest, ctx_->city->id)) {
        memcpy(ctx_->city->id, dest, CITY_ID_LEN);
        ctx_->city->id[CITY_ID_LEN] = 0;
        ctx_->config->dirty = true;
    }
#endif
    return 0;
}
#pragma warn (unused-param, pop)

#pragma warn (unused-param, push, off)
uint8_t config_delete_city_confirmed(void *ctx, uint8_t idx, uint8_t flags) {
    MeteoConfig *config = ((Context*)ctx)->config;
    CityWeather *cw = ((Context*)ctx)->city;
    CityWeather **new_cities = safe_malloc((config->nb_cities - 1) * sizeof(CityWeather*));
    uint8_t i = 0;
    uint8_t j = 0;

    for (; i < config->nb_cities; ++i) {
        if (strcmp(cw->id, config->cities[i]->id) == 0) {
            free(config->cities[i]);
        } else {
            new_cities[j] = config->cities[i];
            ++j;
        }
    }
    free(config->cities);
    config->cities = new_cities;
    config->nb_cities--;
    config->dirty = true;
    return 10; // TODO constant
}

#pragma warn (unused-param, pop)

#pragma warn (unused-param, push, off)
uint8_t config_delete_city(void *ctx, uint8_t idx, uint8_t flags) {
#ifndef NOCONSOLE
    static MenuItem menu_items[] = {
        {"Yes", config_delete_city_confirmed, NULL},
        {"No", previous_menu, NULL},
    };
    Menu menu = {
        "",
        8,  // y
        1, // interlines
        1, // selected
        DEFAULT_MENU_LEFT_PAD,
        2, // total_items
        menu_items,
        config_menu_init3
    };
    if(do_menu(&menu, ctx) == 10) {
        return 10;
    };
#endif
    return 0;
}
#pragma warn (unused-param, pop)

#pragma warn (unused-param, push, off)
uint8_t config_fetch_data(void *ctx, uint8_t idx, uint8_t flags) {
#ifndef NOCONSOLE
/*
    Context *ctx_ = (Context*)ctx;
    char dest[CITY_ID_LEN + 1];
    strcpy(dest, ctx_->city->id);
    clrscr();
    print_config_header();
    printf("\n  City: %s\n\n  Edit the City ID:\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n", ctx_->city->city_name);
    if (flags & ESCAPE_TO_EXIT) {
        printf("Use Arrow keys to move cursor.\n\nPress [Return] to continue.\nPress [Esc] to exit.");
    } else {
        printf("\nUse Arrow keys to move cursor.\n\nPress [Return] to continue.");
    }
    text_input(21, 5, CITY_ID_LEN, dest, ctx_->city->id, ACCEPT_NUMBER | ACCEPT_SPACE);
    clrscr();
    print_config_header();
    if (strcmp(dest, ctx_->city->id)) {
        memcpy(ctx_->city->id, dest, CITY_ID_LEN);
        ctx_->city->id[CITY_ID_LEN] = 0;
        //strcpy(ctx_->city->id, dest);
        ctx_->config->dirty = true;
    }
    */
#endif
    return 0;
}
#pragma warn (unused-param, pop)

bool can_delete_city(void *ctx) {
    return ((Context*)ctx)->config->nb_cities > 1;
}

#pragma warn (unused-param, push, off)
uint8_t config_edit_city(void *ctx, uint8_t idx, uint8_t flags) {
    static MenuItem menu_items[] = {
        {"Edit ID", config_edit_city_id, NULL},
        {"Delete city", config_delete_city, can_delete_city},
        {"Fetch data", config_fetch_data, NULL},
        {"Previous Menu", previous_menu, NULL},
    };
    Menu menu = {
        "",
        12, // y
        1, // interlines
        0, // selected
        DEFAULT_MENU_LEFT_PAD,
        4, // total_items
        menu_items,
        config_menu_init2,
    };
    MeteoConfig *config = (MeteoConfig*)ctx;
    Context new_ctx = {};
    uint8_t rc;
    new_ctx.config = config;
    new_ctx.city = config->cities[idx];
    rc = do_menu(&menu, &new_ctx);
    if (rc == EXIT_CONFIG) {
        return 0;
    }
    return rc;
}
#pragma warn (unused-param, pop)

#pragma warn (unused-param, push, off)
uint8_t config_set_celcius(void *ctx, uint8_t idx, uint8_t flags) {
    MeteoConfig *config = (MeteoConfig*)ctx;
    if (config->default_units == Fahrenheit) {
        config->dirty = true;
    }
    config->default_units = Celsius;
    return EXIT_CONFIG;
}
#pragma warn (unused-param, pop)

#pragma warn (unused-param, push, off)
uint8_t config_set_fahrenheit(void *ctx, uint8_t idx, uint8_t flags) {
    MeteoConfig *config = (MeteoConfig*)ctx;
    if (config->default_units == Celsius) {
        config->dirty = true;
    }
    config->default_units = Fahrenheit;
    return EXIT_CONFIG;
}
#pragma warn (unused-param, pop)

#pragma warn (unused-param, push, off)
uint8_t config_edit_default_units(void *ctx, uint8_t idx, uint8_t flags) {
    static MenuItem menu_items[] = {
        {"Celcius", config_set_celcius},
        {"Fahrenheit", config_set_fahrenheit},
    };
    Menu menu = {
        "",
        4,  // y
        1, // interlines
        0, // selected
        DEFAULT_MENU_LEFT_PAD,
        2, // total_items
        menu_items,
        config_menu_init
    };
    menu.selected = ((MeteoConfig*)ctx)->default_units == Fahrenheit;
    do_menu(&menu, ctx);
    return 0;
}
#pragma warn (unused-param, pop)

#pragma warn (unused-param, push, off)
uint8_t config_do_save_and_exit(void *ctx, uint8_t idx, uint8_t flags) {
    return SAVE_CONFIG;
}
#pragma warn (unused-param, pop)

#pragma warn (unused-param, push, off)
bool config_do_cancel(void *ctx, uint8_t idx, uint8_t flags) {
    return CANCEL_CONFIG;
}
#pragma warn (unused-param, pop)

#pragma warn (unused-param, push, off)
uint8_t previous_menu(void *ctx, uint8_t idx, uint8_t flags) {
    return EXIT_CONFIG;
}
#pragma warn (unused-param, pop)

bool is_dirty(void *ctx) {
    return ((MeteoConfig*)ctx)->dirty;
}

bool is_not_dirty(void *ctx) {
    return !((MeteoConfig*)ctx)->dirty;
}

//#define TOTAL_MENU_ITEMS 8 // TODO variable? sizeof...
MeteoConfig* config_screen(MeteoConfig *config) {
    // work on a copy for easy cancel
    MeteoConfig *copy_config = clone_config(NULL, config);
    static MenuItem menu_items[] = {
        {"Ethernet Slot", config_edit_ethernet_slot},
        {"API Key", config_edit_api_key},
        {"Cities", config_edit_cities},
        {"Default Units", config_edit_default_units},
        {"-", NULL, NULL},
        {"Save and exit configuration", config_do_save_and_exit, is_dirty},
        {"Cancel", config_do_cancel, is_dirty},
        {"Exit configuration", previous_menu, is_not_dirty},
    };
    Menu menu = {
        "",
        4,  // y
        1, // interlines
        0, // selected
        DEFAULT_MENU_LEFT_PAD,
        8, menu_items,
        config_menu_init
    };
    if (do_menu(&menu, copy_config) == SAVE_CONFIG) {
        free(config); // release old config
        config = copy_config;
        save_config(config);
    } else {
        free(copy_config);
    }
    return config;
}
