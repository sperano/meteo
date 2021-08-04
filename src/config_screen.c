#include <conio.h>
#include <ctype.h>
#include <peekpoke.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ip65.h"
#include "config_screen.h"
#include "gfx.h"
#include "net.h"
#include "ui.h"
#include "utils.h"

#pragma static-locals(on)

#define print_config_header() printf("Meteo %s - Configuration\n----------------------------------------", METEO_VERSION)

/**
 * Just clear the screen and display the header
 */
#pragma warn (unused-param, push, off)
void _menu_init_standard(void *ctx) {
#ifndef NOCONSOLE
    clrscr();
#endif
    print_config_header();
}
#pragma warn (unused-param, pop)

/**
 * Clear the screen, display header and city data from context
 */
void _menu_init_city(void *ctx) {
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

/**
 * Clear the screen, display header and confirm delete
 */
void _menu_init_confirm_delete(void *ctx) {
#ifndef NOCONSOLE
    clrscr();
#endif
    print_config_header();
    printf("\n  Are you sure you want to delete\n  %s ?", ((Context*)ctx)->city->name);
}

/**
 *
 */
#pragma warn (unused-param, push, off)
ActionResult config_edit_ethernet_slot(void *ctx, uint8_t idx) {
    ActionResult ar;
#ifndef NOCONSOLE
    char ch;
    bool stop = false;
    uint8_t current_value = ((MeteoConfig*)ctx)->ethernet_slot;
    uint8_t orig_value = current_value;
    void *ptr = (void*)(VideoBases[7]+18);
    char data[1];

    clrscr();
    print_config_header();
    printf("\n  The ethernet card is usually\n  installed in slot #%d.\n\n\n"
           "  Ethernet Slot: #\n\n\n\n\n\n\n\n\n\n\n\n"
           "Enter a number between 1 and 7\nor use the arrow keys.\n\n",
           "Press [Return] to continue.\nPress [Esc] to cancel.", ETH_INIT_DEFAULT);
    while (!stop) {
        data[0] = '0' + current_value;
        memcpy(ptr, data , 1);
        ch = cgetc();
        if (ch >= '1' && ch <= '7') {
            current_value = ch - '0';
        } else if (ch == '\r') {
            ar = EthernetSlotConfigured;
            stop = true;
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
                ar = EditEthernetSlotCancelled;
                stop = true;
                break;
            }
        }
    }
    ((MeteoConfig*)ctx)->ethernet_slot = current_value;
    ((MeteoConfig*)ctx)->dirty = orig_value != current_value;
#endif
    return ar;
}
#pragma warn (unused-param, pop)

/**
 *
 */
#pragma warn (unused-param, push, off)
ActionResult config_edit_api_key(void *ctx, uint8_t idx) {
#ifndef NOCONSOLE
    MeteoConfig *cfg = (MeteoConfig*)ctx;
    char dest[API_KEY_LEN + 1];

    strcpy(dest, cfg->api_key);
    clrscr();
    print_config_header();
    printf("\n  Generate an API Key at:\n  https://openweathermap.org\n\n"
           "  The API Key is a 32 characters long\n  hexadecimal string.\n\n\n\n"
           "  API Key:\n\n\n\n\n\n\n\n\n"
           "Use Arrow keys to move cursor.\n\nPress [Return] to continue.\nPress [Esc] to cancel.");
    if (text_input(2, 13, API_KEY_LEN, dest, cfg->api_key, ACCEPT_HEXA | ACCEPT_ESCAPE) == -1) {
        return EditAPIKeyCancelled;
    }
    if (strcmp(dest, cfg->api_key)) {
        strcpy(cfg->api_key, dest);
        cfg->dirty = true;
    }
#endif
    return APIKeyConfigured;
}
#pragma warn (unused-param, pop)

/**
 *
 */
#pragma warn (unused-param, push, off)
ActionResult config_edit_cities(void *ctx, uint8_t idx) {
    ActionResult ar;
    MeteoConfig *config = (MeteoConfig*)ctx;
    uint8_t nb_cities = 0;
    uint8_t i = 0;
    uint8_t selected = 0;
    MenuItem *menu_items;
    do {
        nb_cities = config->nb_cities;
        menu_items = safe_malloc((nb_cities + 4) * sizeof(MenuItem));
        for(i = 0; i < nb_cities; ++i) {
            menu_items[i].name = config->cities[i]->name;
            menu_items[i].action = config_edit_city;
            menu_items[i].visibility_check = NULL;
        }
        menu_items[nb_cities].name = "-";
        menu_items[nb_cities].visibility_check = NULL;
        menu_items[nb_cities + 1].name = "Add a city";
        menu_items[nb_cities + 1].action = config_add_city;
        menu_items[nb_cities + 1].visibility_check = NULL;

        menu_items[nb_cities + 2].name = "-";
        menu_items[nb_cities + 2].action = NULL;
        menu_items[nb_cities + 2].visibility_check = NULL;
        menu_items[nb_cities + 3].name = "Previous menu";
        menu_items[nb_cities + 3].action = previous_menu;
        menu_items[nb_cities + 3].visibility_check = NULL;
        ar = do_menu(4, &selected, menu_items, nb_cities + 4, _menu_init_standard, ctx);
        switch(ar) {
        case CityAdded:
            selected = config->nb_cities - 1;
            break;
        case CityDeleted:
            if (selected > 0) {
                selected--;
            }
        }
        free(menu_items);
    }
    while (ar != PreviousMenu);
    return ar;
}

#pragma warn (unused-param, pop)

#pragma warn (unused-param, push, off)
ActionResult config_edit_city_id(void *ctx, uint8_t idx) {
#ifndef NOCONSOLE
    Context *ctx_ = (Context*)ctx;
    char dest[CITY_ID_LEN + 1];
    strcpy(dest, ctx_->city->id);
    clrscr();
    print_config_header();
    printf("\n  City: %s\n\n  Edit the City ID:\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
          "Use Arrow keys to move cursor.\n\n"
          "Press [Return] to continue.\nPress [Esc] to cancel.", ctx_->city->name);
    if (text_input(21, 5, CITY_ID_LEN, dest, ctx_->city->id, ACCEPT_NUMBER | ACCEPT_SPACE | ACCEPT_ESCAPE) == -1) {
        return EditCityIDCancelled;
    };
    clrscr();
    print_config_header();
    if (strcmp(dest, ctx_->city->id)) {
        memcpy(ctx_->city->id, dest, CITY_ID_LEN);
        ctx_->city->id[CITY_ID_LEN] = 0;
        ctx_->config->dirty = true;
    }
#endif
    return CityIDConfigured;
}
#pragma warn (unused-param, pop)

#pragma warn (unused-param, push, off)
ActionResult config_delete_city_confirmed(void *ctx, uint8_t idx) {
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
    return CityDeleted;
}

#pragma warn (unused-param, pop)

#pragma warn (unused-param, push, off)
ActionResult config_delete_city(void *ctx, uint8_t idx) {
#ifndef NOCONSOLE
    static MenuItem menu_items[] = {
        {"Yes", config_delete_city_confirmed, NULL},
        {"No", previous_menu, NULL},
    };
    uint8_t selected;
    return do_menu(8, &selected, menu_items, 2, _menu_init_confirm_delete, ctx);
#else
    return PreviousMenu;
#endif
}
#pragma warn (unused-param, pop)

#pragma warn (unused-param, push, off)
ActionResult config_fetch_data(void *ctx, uint8_t idx) {
#ifndef NOCONSOLE
    Context *ctx_ = (Context*)ctx;
    MeteoConfig *cfg = ctx_->config;
    CityWeather *city = ctx_->city;
    if (!download_weather_data(cfg->api_key, city)) {
        return FetchDataFailed;
    }
    city->bitmap = get_bitmap_for_icon(city->icon);
    cfg->dirty = true;
#endif
    return DataFetched;
}
#pragma warn (unused-param, pop)

bool can_delete_city(void *ctx) {
    return ((Context*)ctx)->config->nb_cities > 1;
}

#pragma warn (unused-param, push, off)
ActionResult config_edit_city(void *ctx, uint8_t idx) {
    static MenuItem menu_items[] = {
        {"Edit ID", config_edit_city_id, NULL},
        {"Delete city", config_delete_city, can_delete_city},
        {"Fetch data", config_fetch_data, NULL},
        {"-", NULL, NULL},
        {"Previous Menu", previous_menu_city, NULL},
    };
    MeteoConfig *config = (MeteoConfig*)ctx;
    ActionResult ar = 0;
    uint8_t selected = 0;
    Context new_ctx = {};
    new_ctx.config = config;
    new_ctx.city = config->cities[idx];
    do {
        ar = do_menu(12, &selected, menu_items, 5, _menu_init_city, &new_ctx);
    }
    while(ar != CityDeleted && ar != PreviousMenuCity);
    return ar;
}
#pragma warn (unused-param, pop)

#pragma warn (unused-param, push, off)
ActionResult config_add_city(void *ctx, uint8_t idx) {
    MeteoConfig *config = (MeteoConfig*)ctx;
    char dest[CITY_ID_LEN + 1];
    CityWeather *city;

    memset(dest, ' ', CITY_ID_LEN);
    dest[CITY_ID_LEN] = 0;

    clrscr();
    print_config_header();
    printf("\n  New City ID:\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    printf("\nUse Arrow keys to move cursor.\n\nPress [Return] to continue.\n\nPress [Esc] to cancel.");
    if (text_input(17, 3, CITY_ID_LEN, dest, dest, ACCEPT_NUMBER | ACCEPT_SPACE | ACCEPT_ESCAPE) == -1) {
        return CityAddCancelled;
    };

    city = safe_malloc(sizeof(CityWeather));
    strcpy(city->id, dest);
    if (!download_weather_data(config->api_key, city)) {
        free(city);
        clrscr();
        print_config_header();
        printf("\n  New City ID:   %s\n", dest);
        printf("\n\nFailed to download weather data.\n\nPress any key to continue.\n");
        cgetc();
        return CityAddFailed;
    }
    city->bitmap = get_bitmap_for_icon(city->icon);

    config->nb_cities++;
    config->cities = safe_realloc(config->cities, config->nb_cities * sizeof(CityWeather*));
    config->cities[config->nb_cities - 1] = city;
    config->dirty = true;
    clrscr();
    print_config_header();
    return CityAdded;
}
#pragma warn (unused-param, pop)

#pragma warn (unused-param, push, off)
ActionResult config_set_celcius(void *ctx, uint8_t idx) {
    MeteoConfig *config = (MeteoConfig*)ctx;
    if (config->default_units == Fahrenheit) {
        config->dirty = true;
    }
    config->default_units = Celsius;
    return UnitConfigured;
}
#pragma warn (unused-param, pop)

#pragma warn (unused-param, push, off)
ActionResult config_set_fahrenheit(void *ctx, uint8_t idx) {
    MeteoConfig *config = (MeteoConfig*)ctx;
    if (config->default_units == Celsius) {
        config->dirty = true;
    }
    config->default_units = Fahrenheit;
    return UnitConfigured;
}
#pragma warn (unused-param, pop)

#pragma warn (unused-param, push, off)
ActionResult config_edit_default_units(void *ctx, uint8_t idx) {
    static MenuItem menu_items[] = {
        {"Celcius", config_set_celcius},
        {"Fahrenheit", config_set_fahrenheit},
    };
    uint8_t selected = ((MeteoConfig*)ctx)->default_units == Fahrenheit;
    return do_menu(4, &selected, menu_items, 2, _menu_init_standard, ctx);
}
#pragma warn (unused-param, pop)

#pragma warn (unused-param, push, off)
ActionResult save_and_exit_config_screen(void *ctx, uint8_t idx) {
    return SaveAndExitConfig;
}
#pragma warn (unused-param, pop)

#pragma warn (unused-param, push, off)
ActionResult cancel_config_screen(void *ctx, uint8_t idx) {
    return CancelAndExitConfig;
}
#pragma warn (unused-param, pop)

#pragma warn (unused-param, push, off)
ActionResult exit_config_screen(void *ctx, uint8_t idx) {
    return ExitConfig;
}
#pragma warn (unused-param, pop)

#pragma warn (unused-param, push, off)
ActionResult previous_menu(void *ctx, uint8_t idx) {
    return PreviousMenu;
}
#pragma warn (unused-param, pop)

#pragma warn (unused-param, push, off)
ActionResult previous_menu_city(void *ctx, uint8_t idx) {
    return PreviousMenuCity;
}
#pragma warn (unused-param, pop)

bool is_dirty(void *ctx) {
    return ((MeteoConfig*)ctx)->dirty;
}

bool is_not_dirty(void *ctx) {
    return !((MeteoConfig*)ctx)->dirty;
}

MeteoConfig* config_screen(MeteoConfig *config) {
    // work on a copy for easy cancel
    MeteoConfig *copy_config = clone_config(NULL, config);
    static MenuItem menu_items[] = {
        {"Ethernet Slot", config_edit_ethernet_slot},
        {"API Key", config_edit_api_key},
        {"Cities", config_edit_cities},
        {"Default Units", config_edit_default_units},
        {"-", NULL, NULL},
        {"Save and exit configuration", save_and_exit_config_screen, is_dirty},
        {"Cancel", cancel_config_screen, is_dirty},
        {"Exit configuration", exit_config_screen, is_not_dirty},
    };
    bool stop = false;
    uint8_t selected = 0;
    while (!stop) {
        switch(do_menu(4, &selected, menu_items, 8, _menu_init_standard, copy_config)) {
        case SaveAndExitConfig:
            free(config); // release old config
            config = copy_config;
            save_config(config);
            stop = true;
            break;
        case CancelAndExitConfig:
        case ExitConfig:
            free(copy_config);
            stop = true;
            break;
        }
    }
    return config;
}
