#include <conio.h>
#include <ctype.h>
#include <peekpoke.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
|  5 |  6 |  7 |  8 |  9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 20 | 21 | 22 | 23 | 24 | 25 | 26 | 27 | 28 | 29 | 30 | 31 | 32 | 33 | 34 | 35 | 36 | AppId
+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
| 37 | Cities count
+----+----+----+----+----+----+----+----+
| 38 | 39 | 40 | 41 | 42 | 43 | 44 | 45 | CityID #1 (Always 8 bytes)
+----+----+----+----+----+----+----+----+
| 46 | 47 | 48 | 49 | 50 | 51 | 52 | 53 | CityID #2
+----+----+----+----+----+----+----+----+
 Etc...

*/

MeteoConfig config;

void save_config() {
    uint8_t byte, i, j;
    FILE *file = fopen(METEO_CONFIG_FILENAME, "w");
    if (file == NULL) {
        fail("Can't open config file\n");
    }
    putc(0xe5, file);
    putc(0x76, file);
    putc(0x01, file); // version
    putc(config.ethernet_slot, file);
    for (i = 0; i < 32; ++i) {
        putc(config.app_id[i], file);
    }
    putc(config.nb_cities, file);
    for (i = 0; i < config.nb_cities; ++i) {
        for (j = 0; j < 8; ++j) {
            putc(config.city_ids[i][j], file);
        }
    }
    fclose(file);
}

void read_config() {
    uint8_t byte, i, j;
    char *appid = config.app_id;
    FILE *file;

    //print_line();
    printf("Loading config %s\n", METEO_CONFIG_FILENAME);
    file = fopen(METEO_CONFIG_FILENAME, "r");
    if (file == NULL) {
        fail("Can't open config file\n");
    }
    if ((byte = getc(file)) != 0xe5) { // magic number
        fail("Not a valid config file\n");
    }
    if ((byte = getc(file)) != 0x76) { // magic number
        fail("Not a valid config file\n");
    }
    getc(file); // version
    config.ethernet_slot = getc(file); // ethernet slot
    for (i = 0; i < 32; ++i) {
        config.app_id[i] = getc(file); // app id
    }
    config.app_id[32] = 0;

    config.nb_cities = getc(file);
    //printf("nb_cities=%d\n", config.nb_cities);
    config.city_ids = safe_malloc(config.nb_cities * sizeof(char*), "Array of CityID");
    for (i = 0; i < config.nb_cities; ++i) {
        config.city_ids[i] = safe_malloc(9 * sizeof(char), "CityID");
        for (j = 0; j < 8; ++j) {
            config.city_ids[i][j] = getc(file); // city id
        }
        config.city_ids[i][8] = 0;
    }
    fclose(file);
    //return &config;
}

void free_config() {
    uint8_t i;
    for (i = 0; i < config.nb_cities; ++i) {
        free(config.city_ids[i]);
    }
    free(config.city_ids);
}

void print_config() {
    uint8_t i;
    printf("Ethernet Slot: %d\n", config.ethernet_slot);
    printf("AppID: %s\n", config.app_id);
    for (i = 0; i < config.nb_cities; ++i) {
        if (config.city_ids[i][0]) {
            printf("CityID[%d]: %s\n", i, config.city_ids[i]);
        }
    }
}

void validate_config() {
    //uint8_t at_least_one = 0;
    //uint8_t i;
    if (strlen(config.app_id) != 32) {
        fail("Invalid Weather App Id: '%s'\n", config.app_id);
    }
    if (config.nb_cities == 0) {
        fail("No cities configured.\n");
    }
}

void draw_menu(uint8_t selected, uint8_t choices, MenuItem config_menu[]) {
    char buffer[40];
    uint8_t i, j, k;
    char c;
    for (i = 0; i < choices; ++i) {
        if (config_menu[i].name[0] != '-') {
            strcpy(buffer, config_menu[i].name);
            k = strlen(config_menu[i].name);
            if (i == selected) {
                for (j = 0; j < k; ++j) {
                    c = buffer[j];
                    if (isupper(c) || c == '[' || c == ']') {
                        buffer[j] -= 0x40;
                    }
                }
            } else {
                for (j = 0; j < k; ++j) {
                    buffer[j] += 0x80;
                }
            }
            memcpy((void *)(VideoBases[5 + (i*2)]), buffer, strlen(buffer));
        }
    }
}

uint8_t config_do_edit_app_id(void) {
    char ch;
    char id[32];
    char *idptr = config.app_id;
    uint8_t exit = 0;
    uint8_t pos = 0;
    //uint8_t current_value = config.ethernet_slot;
    void *ptr = (void*)(VideoBases[7]+7);

    memcpy(id, config.app_id, 32);
    for (ch = 0; ch < 32; ++ch) {
        if (id[ch] >= 'A' && id[ch] <= 'F') {
            id[ch] -= 0x40;
        }
    }
    clrscr();
    printf("Meteo %s - Configuration\n\n\n\n\n\n\n\n\nAppID:\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nPress [Return] to exit.", METEO_VERSION);
    while (!exit) {
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
                exit = 1;
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
            }
        }

    }
    for (ch = 0; ch < 32; ++ch) {
        idptr[ch] = id[ch];
        if (idptr[ch] >= ('A'-0x40) && idptr[ch] <= ('F'-0x40)) {
            idptr[ch] += 0x40;
        }
    }
    //config.ethernet_slot = current_value;
    return 0;
}

uint8_t config_do_edit_ethernet(void) {
    char ch;
    uint8_t current_value = config.ethernet_slot;
    void *ptr = (void*)(VideoBases[5]+15);
    char data[1];

    clrscr();
    printf("Meteo %s - Configuration\n\n\n\n\nEthernet Slot:\n\n\n\n\n\n\n\n\n\n\n\n\n\nEnter number or use arrow keys.\n\n\n\nPress [Return] to exit.", METEO_VERSION);
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
            }
        }
    }
    config.ethernet_slot = current_value;
    return 0;
}

uint8_t config_do_edit_cities(void) {
    printf("Yep 3!");
    return 0;
}

uint8_t config_do_quit(void) {
    save_config();
    return 1;
}

uint8_t config_do_cancel(void) {
    free_config();
    read_config();
    return 1;
}

#define TOTAL_MENU_ITEMS 7
void config_screen() {
    static MenuItem config_menu[] = {
        {"Ethernet Slot: 3", config_do_edit_ethernet},
        {"AppID: 12345678901234567890123456789012", config_do_edit_app_id},
        {"Cities (99)", config_do_edit_cities},
        {"-"},
        {"-"},
        {"Save and quit configuration", config_do_quit},
        {"Cancel", config_do_cancel},
    };
    uint8_t selected = 6;
    char ch;
    uint8_t exit = 0;

    while (!exit) {
        clrscr();
        printf("Meteo %s - Configuration\n", METEO_VERSION);
        sprintf(config_menu[0].name, "Ethernet Slot: %d", config.ethernet_slot);
        sprintf(config_menu[1].name, "AppID: %s", config.app_id);
        sprintf(config_menu[2].name, "Cities (%d)", config.nb_cities);
        draw_menu(selected, TOTAL_MENU_ITEMS, config_menu);
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
            exit = config_menu[selected].action();
            break;
        }
    }
}
