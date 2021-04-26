#include <peekpoke.h>
#include <stdio.h>
#include <string.h>
#include "bitmaps.h"
#include "gfx.h"
#include "utils.h"

uint16_t VideoBases[] = {
    0x400, 0x480, 0x500, 0x580, 0x600, 0x680, 0x700, 0x780,
    0x428, 0x4a8, 0x528, 0x5a8, 0x628, 0x6a8, 0x728, 0x7a8,
    0x450, 0x4d0, 0x550, 0x5d0, 0x650, 0x6d0, 0x750, 0x7d0,
};

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
    static const char menu_str[] = { 0x20, 0x83, 0x6f, 0x6e, 0x66, 0x69, 0x67, 0x75, 0x72, 0x65, 0x20, 0x7c, 0x20, 0x03, 0x68, 0x61, 0x6e, 0x67, 0x65, 0x20,
                                     0x95, 0x6e, 0x69, 0x74, 0x73, 0x20, 0x7c, 0x20, 0x91, 0x75, 0x69, 0x74, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20 };
    //memcpy((void *)VideoBases[22], ' ' + 0x80, 40); TODO not necessary?
    memcpy((void *)VideoBases[23], menu_str, 40);
}

void update_gfx_text(CityWeather *cw, enum Units units) {
    static char line1[41] = {0};
    static char line2[41] = {0};
    static char line3[41] = {0};
    uint8_t i;

    // first line
    strcpy(line1, cw->city_name);
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
    Bitmap *ptr = cw->bitmap;
    for (i = 0; i < 20; ++i) {
        memcpy((void*)VideoBases[i], (*ptr)[i], 40);
    }
}

#define MAX_BITMAPS 6
Bitmap* get_bitmap_for_icon(char *icon) {
    static IconBitmapPair iconBitmapMap[] = {
        {"01d", &Bitmap01d},
        {"01n", &Bitmap01n},
        {"02d", &Bitmap02d},
        {"02n", &Bitmap02n},
        {"04d", &Bitmap04d},
        {"04n", &Bitmap04d},
    };
    uint8_t i;
    for (i = 0; i < MAX_BITMAPS; ++i) {
        if (!strcmp(iconBitmapMap[i].icon, icon)) {
            return iconBitmapMap[i].bitmap;
        }
    }
    return &Bitmap404;
}


/*
void pset(unsigned char x, unsigned char y, unsigned char color) {
    uint16_t addr = VideoBases[y >> 1] + x;
    uint8_t byte = PEEK(addr);

    if (y & 1) {
        byte |= color << 4;
    } else {
        byte |= color;
    }
    POKE(addr, byte);
}
*/
