#include <peekpoke.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bitmaps.h"
#include "gfx.h"
#include "utils.h"

void init_gfx(void) {
    POKE(TEXTOFF, 0);
    POKE(HIRESOFF, 0);
    POKE(MIXEDON, 0);
    POKE(PAGE2OFF, 0);
    //POKE(0xC00D, 0); // 80COLON
    //POKE(0xC05E, 0); // SETAN3
}

void exit_gfx(void) {
    POKE(TEXTON, 0);
}

void clear_screen() {
    memset((void *)VideoBases[0], 0, 0x400);
    memset((void *)VideoBases[20], ' ' + 0x80, 40);
    memset((void *)VideoBases[21], ' ' + 0x80, 40);
    memset((void *)VideoBases[22], ' ' + 0x80, 40);
    memset((void *)VideoBases[23], ' ' + 0x80, 40);
}

void set_menu_text(void) {
    static const char menu_str[] = {
        0x20, 0x83, 0x6f, 0x6e, 0x66, 0x69, 0x67, 0x75, 0x72, 0x65,
        0x20, 0x7c, 0x20, 0x95, 0x6e, 0x69, 0x74, 0x73, 0x20, 0x7c,
        0x20, 0x92, 0x65, 0x66, 0x72, 0x65, 0x73, 0x68, 0x20, 0x7c,
        0x20, 0x91, 0x75, 0x69, 0x74, 0x20, 0x20, 0x20, 0x20, 0x20,
    };
    memcpy((void *)VideoBases[23], menu_str, 40);
}

void update_gfx_text(CityWeather *cw, Units units) {
    static char line1[41] = {0};
    static char line2[41] = {0};
    static char line3[41] = {0};
    uint8_t i;

    // first line
    strcpy(line1, cw->name);
    strcat(line1, ": ");
    if (units == Celsius) {
        celsius_str(line1 + strlen(line1), cw->temperatureC);
        strcat(line1, "C");
    } else {
        sprintf(line1 + strlen(line1), "%dF", cw->temperatureF);
    }
    i = strlen(line1);
    memset(line1 + i, ' ', 40 - i);
    // second line
    strcpy(line2, cw->weather );
    strcat(line2, " (");
    strcat(line2, cw->description);
    strcat(line2, ")");
    i = strlen(line2);
    memset(line2 + i, ' ', 40 - i);
    // third line
    if (units == Celsius) {
        strcpy(line3, "Minimum: ");
        celsius_str(line3 + strlen(line3), cw->minimumC);
        strcat(line3, "C  /  Maximum: ");
        celsius_str(line3 + strlen(line3), cw->maximumC);
        strcat(line3, "C");
        i = strlen(line3);
        memset(line3 + i, ' ', 40 - i);
    } else {
        sprintf(line3, "Minimum: %dF  /  Maximum: %dF", cw->minimumF, cw->maximumF);
        i = strlen(line3);
        memset(line3 + i, ' ', 40 - i);
    }
    // prepare to display
    for (i = 0; i < 40; ++i) {
        line1[i] += 0x80;
        line2[i] += 0x80;
        line3[i] += 0x80;
    }
    // display
    memcpy((void *)VideoBases[20], line1, 40);
    memcpy((void *)VideoBases[21], line2, 40);
    memcpy((void *)VideoBases[22], line3, 40);
}

void update_gfx_image(CityWeather *cw) {
    uint8_t i;
    Bitmap ptr = cw->bitmap;
    for (i = 0; i < 20; ++i) {
        memcpy((void*)VideoBases[i], ptr + (i * 20), 40);
    }
}

#define BITMAP_SIZE 800
Bitmap load_bitmap(char *filename) {
    char path[10];
    uint8_t *data = safe_malloc(BITMAP_SIZE);
    FILE *file;
    uint16_t i;

    strcpy(path, filename);
    strcat(path, ".A2LR");
    //printf("Loading %s\n", path);

    file = fopen(path, "r");
    if (file == NULL) {
        fail(FailOpenBitmapFileRead, 0);
    }
    i = fread(data, BITMAP_SIZE, 1, file);
    if (i != 1) {
        fclose(file);
        fail(FailBitmapElementCount, 0);
    }
    //printf("Bitmap %s loaded\n", path);
    fclose(file);
    return (Bitmap)data;
}

//Bitmap
char *get_filename_for_icon(char *icon) {
    static IconMapping mappings[] = {
        {"01d", "I01D"},
        {"01n", "I01N"},
        {"02d", "I02D"},
        {"02n", "I02N"},
        {"03d", "I03D"},
        {"03n", "I03N"},
        {"04d", "I04D"},
        {"04n", "I04N"},
        {"09d", "I09D"},
        {"09n", "I09N"},
        {"10d", "I10D"},
        {"10n", "I10N"},
        {"11d", "I11D"},
        {"11n", "I11N"},
        {"13d", "I13D"},
        {"13n", "I13N"},
    };
    uint8_t i;
    //uint8_t max_bitmaps = sizeof(mappings) / sizeof(IconMapping);
    for (i = 0; i < 16; ++i) {
        if (!strcmp(mappings[i].icon, icon)) {
            return mappings[i].filename;
        }
    }
    return "I404";
}

Bitmap get_bitmap_for_icon(char *icon) {
    static BitmapMapping *bitmap_mappings = NULL;
    static uint8_t bm_count = 0;
    char *filename = get_filename_for_icon(icon);
    Bitmap bitmap;
    uint8_t i;

    if (bitmap_mappings != NULL) {
        for (i = 0; i < bm_count; ++i) {
            if (!strcmp(bitmap_mappings[i].filename, filename)) {
                return bitmap_mappings[i].bitmap;
            }
        }
    }
    if (bitmap_mappings == NULL) {
        bitmap_mappings = safe_malloc(sizeof(BitmapMapping));
    } else {
        bitmap_mappings = safe_realloc(bitmap_mappings, (bm_count+1)*sizeof(BitmapMapping));
    }

    bitmap = load_bitmap(filename);
    bitmap_mappings[bm_count].filename = filename;
    bitmap_mappings[bm_count].bitmap = bitmap;
    bm_count++;
    return bitmap;
}
