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

void fail(char *msg) {
    perror(msg);
    for(;;); //TODO
    //exit(1);
}

void* safe_malloc(size_t size) {
    void *p = malloc(size);
    if (p == NULL) {
        printf("size: %d\n", size);
        fail("safe_malloc failed, Out of memory");
    }
    return p;
}
