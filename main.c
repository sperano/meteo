// check https://github.com/pedgarcia/a2graph/blob/master/a2graph.c
// check https://github.com/ppelleti/json65
// 01 to 04, then  09 to 013
// http://openweathermap.org/img/w/01d.png
// http://openweathermap.org/img/w/01n.png
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <apple2.h>
#include <conio.h>
#include <peekpoke.h>
#include "vendor/json65-master/src/json65-file.h"
#include "vendor/json65-master/src/json65-tree.h"
#include "meteo.h"

unsigned int VideoBases[] = {
    0x400,
    0x480,
    0x500,
    0x580,
    0x600,
    0x680,
    0x700,
    0x780,
    0x428,
    0x4a8,
    0x528,
    0x5a8,
    0x628,
    0x6a8,
    0x728,
    0x7a8,
    0x450,
    0x4d0,
    0x550,
    0x5d0,
    0x650,
    0x6d0,
    0x750,
    0x7d0,
};

void clear_screen() {
    memset((void *)VideoBases[0], 0, 0x400);
    //memset((void *)0x650, 'A' + 0x80, 40);
    //memset((void *)0x750, 'A' + 0xa0, 40);
    memset((void *)VideoBases[20], ' ' + 0x80, 40);
    memset((void *)VideoBases[21], ' ' + 0x80, 40);
    memset((void *)VideoBases[22], ' ' + 0x80, 40);
    memset((void *)VideoBases[23], ' ' + 0x80, 40);
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

int main_test1(void) {
    uint8_t x;

    POKE(TEXTOFF, 0);
    POKE(HIRESOFF, 0);
    POKE(MIXEDON, 0);
    POKE(PAGE2OFF, 0);
    clear_screen();
    for(x = 0; x < 40; x++) {
        pset(x, x, 9);
        pset(x, 39-x, 3);
        pset(x, 20, 4);
        pset(20, x, 6);
    }

    while(!kbhit());

    POKE(TEXTON, 0);
    return 0;
}
*/

int main_test2(void) {
    uint8_t y;

    POKE(TEXTOFF, 0);
    POKE(HIRESOFF, 0);
    POKE(MIXEDON, 0);
    POKE(PAGE2OFF, 0);
    //POKE(0xC00D, 0); // 80COLON
    //POKE(0xC05E, 0); // SETAN3
    clear_screen();

    for(y = 0; y < 20; y++) {
        memcpy((void *)VideoBases[y], Bitmap[y], 40);
    }
    memcpy((void *)VideoBases[20], TxtLine1, 14);
    memcpy((void *)VideoBases[21], TxtLine2, 9);
    memcpy((void *)VideoBases[22], TxtLine3, 17);
    //memcpy((void *)VideoBases[23], TxtLine2, 5);
    while(!kbhit());

    POKE(TEXTON, 0);
    clrscr();
    return 0;
}

int main_test3(void) {
    const char *filename = "WEATHER.JSON";
    FILE *f;
    CityWeather cw;
    int rc;

    printf("\nLoading %s...\n", filename);
    f = fopen(filename, "r");
    if (!f) {
        perror(filename);
        return 1;
    }
    rc = parse_api_response(&cw, f);
    if (rc == 0) {
        printf("City: %s\n", cw.city_name);
        printf("Weather: %s\n", cw.weather);
        printf("Description: %s\n", cw.description);
        printf("Icon: %s\n", cw.icon);
        printf("Temperature: %d\n", cw.temperature);
        printf("Minimum: %d\n", cw.minimum);
        printf("Maximum: %d\n", cw.maximum);
        printf("Humidity: %d\n", cw.humidity);
    }
    free(cw.city_name);
    free(cw.weather);
    free(cw.description);
    free(cw.icon);
    printf("\n\nPress any key...");
    cgetc();
    return main_test2();
}

int main(void) {
    //return main_test1();
    //return main_test2();
    return main_test3();
}
