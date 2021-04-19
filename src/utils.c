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
    int l;
    char *txtline = cw->text_lines[0];
    char *p;
    // first line //
    strcpy(txtline, cw->city_name);
    strcat(txtline, ": ");
    strcat(txtline, cw->weather);
    strcat(txtline, " (");
    strcat(txtline, cw->description);
    strcat(txtline, ")");
    l = strlen(txtline);
    memset(txtline + l, ' ', 40 - l);
    txtline[40] = 0;
    // 2nd line
    txtline = cw->text_lines[1];
    celsius_str(txtline, cw->temperatureC);
    strcat(txtline, "C  /  Min: ");
    p = txtline + strlen(txtline);
    celsius_str(p, cw->minimumC);
    strcat(txtline, "C  /  Max: ");
    p = txtline + strlen(txtline);
    celsius_str(p, cw->maximumC);
    strcat(txtline, "C");
    l = strlen(txtline);
    memset(txtline + l, ' ', 40 - l);
    txtline[40] = 0;
    // 3rd line
    //txtline = cw->text_lines[2];
    //memset(txtline, ' ', 40);
    //txtline[40] = 0;
    // 4th line
    //txtline = cw->text_lines[3];
    //strcpy(txtline, "C:Configure | U:Unit | Q:Quit");
    //l = strlen(txtline);
    //memset(txtline + l, ' ', 40 - l);
    //txtline[40] = 0;
}

void fail(char *msg) {
    printf("%s\n", msg);
    exit(1);
}

void* safe_malloc(size_t size) {
    void *p = malloc(size);
    if (p == NULL) {
        fail("Out of memory");
    }
    return p;
}
