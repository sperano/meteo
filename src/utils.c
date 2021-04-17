#include <stdlib.h>
#include <string.h>
#include "utils.h"

uint16_t get_int(const char *src) {
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
