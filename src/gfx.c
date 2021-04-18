#include <peekpoke.h>
#include <stdio.h>
#include <string.h>
#include "gfx.h"
#include "utils.h"

unsigned int VideoBases[] = {
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

void prepare_gfx_text(CityWeather *cw) {
    int i, j;
    prepare_text(cw);
    for (i = 0; i < 4 ; ++i) {
        for (j = 0; j < 40; ++j) {
            cw->text_lines[i][j] += 0x80;
        }
    }
}

void update_gfx_text(CityWeather *cw) {
    uint8_t i;
    for (i = 0; i < 4; ++i) {
        memcpy((void *)VideoBases[20 + i], cw->text_lines[i], 40);
    }
}

void update_gfx_image(CityWeather *cw) {
    uint8_t i;
    for (i = 0; i < 20; ++i) {
        memcpy((void*)VideoBases[i], (*cw->bitmap)[i], 40);
    }
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
