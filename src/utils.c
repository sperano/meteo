#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

uint16_t str_to_int(const char *src) {
    uint16_t value = 0;
    uint8_t dec = 0;

    char *ptr_token = strchr(src, '.');
    if (ptr_token == NULL) {
        value = atoi(src);
    } else {
        *ptr_token = 0;
        value = atoi(src);
        dec = ptr_token[1] - '0';
        if (dec >= 5) {
            value++;
        }
    }
    return value;
}

// 273.15 == 27315
kelvin str_to_kelvin(const char *src) {
    uint16_t value = 0;
    char *ptr_token = strchr(src, '.');
    if (ptr_token == NULL) {
        // no decimal
        return atoi(src) * 100;
    }
    *ptr_token = 0;
    ptr_token++;
    value = atoi(ptr_token);
    if (strlen(ptr_token) == 1) {
        value *= 10;
    }
    return (atoi(src) * 100) + value;
}

// 2 decimals to 1 decimal
celsius kelvin_to_celsius(const kelvin temp) {
    return (temp - 27315 + 5) / 10;
}

// 1 decimal to no decimal
fahrenheit celsius_to_fahrenheit(const celsius temp) {
    return (((temp * 18) + 50)/100)+32;
}

void celsius_str(char *buffer, const celsius temp) {
    uint8_t i, j;
    i = temp / 10;
    j = temp % 10;
    if (j == 0) {
        snprintf(buffer, 5, "%d", i);
    } else {
        snprintf(buffer, 5, "%d.%d", i, j);
    }
}

void prepare_text(CityWeather *cw) {
    int i, l;
    char *p;
    strcpy(cw->text_lines[0], cw->city_name);
    strcat(cw->text_lines[0], ": ");
    strcat(cw->text_lines[0], cw->weather);
    strcat(cw->text_lines[0], " (");
    strcat(cw->text_lines[0], cw->description);
    strcat(cw->text_lines[0], ")");
    l = strlen(cw->text_lines[0]);
    for (i = l; i < 40; ++i) {
        cw->text_lines[0][i] = ' ';
    }
    cw->text_lines[0][40] = 0;
    // line 1
    memset(cw->text_lines[1], ' ', 40);
    cw->text_lines[1][40] = 0;
    // line 2
    celsius_str(cw->text_lines[2], cw->temperatureC);
    strcat(cw->text_lines[2], "C");
    l = strlen(cw->text_lines[2]);
    for (i = l; i < 8; ++i) {
        cw->text_lines[2][i] = ' ';
    }
    cw->text_lines[2][i] = 0;
    strcat(cw->text_lines[2], "Min: ");
    p = cw->text_lines[2] + strlen(cw->text_lines[2]);
    celsius_str(p, cw->minimumC);
    strcat(cw->text_lines[2], "C");
    l = strlen(cw->text_lines[2]);
    for (i = l; i < 21; ++i) {
        cw->text_lines[2][i] = ' ';
    }
    cw->text_lines[2][i] = 0;
    strcat(cw->text_lines[2], "Max: ");
    p = cw->text_lines[2] + strlen(cw->text_lines[2]);
    celsius_str(p, cw->maximumC);
    strcat(cw->text_lines[2], "C");
    l = strlen(cw->text_lines[2]);
    for (i = l; i < 40; ++i) {
        cw->text_lines[2][i] = ' ';
    }
    cw->text_lines[2][i] = 0;
    // line 3
    sprintf(cw->text_lines[3], "%dF", cw->temperatureF);
    l = strlen(cw->text_lines[3]);
    for (i = l; i < 13; ++i) {
        cw->text_lines[3][i] = ' ';
    }
    cw->text_lines[3][i] = 0;
    p = cw->text_lines[3] + strlen(cw->text_lines[3]);
    sprintf(p, "%dF", cw->minimumF);
    l = strlen(cw->text_lines[3]);
    for (i = l; i < 26; ++i) {
        cw->text_lines[3][i] = ' ';
    }
    cw->text_lines[3][i] = 0;
    p = cw->text_lines[3] + strlen(cw->text_lines[3]);
    sprintf(p, "%dF", cw->maximumF);
    l = strlen(cw->text_lines[3]);
    for (i = l; i < 40; ++i) {
        cw->text_lines[3][i] = ' ';
    }
    cw->text_lines[3][i] = 0;
}
