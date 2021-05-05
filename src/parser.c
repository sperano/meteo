#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json65-file.h"
#include "json65-tree.h"
#include "config.h"
#include "parser.h"
#include "utils.h"

#pragma static-locals(on)

typedef enum {
    PARSER_INIT,
    PARSER_GET_WEATHER,
    PARSER_GET_DESCRIPTION,
    PARSER_GET_ICON,
    PARSER_GET_TEMP,
    PARSER_GET_FEELS_LIKE,
    PARSER_GET_MINIMUM,
    PARSER_GET_MAXIMUM,
    PARSER_EXP_CITY_NAME,
    PARSER_GET_CITY_NAME,
    PARSER_END
} ParserState;

static ParserState current_state;

char* alloc_cpy(const char *src) {
    char *dest = safe_malloc(strlen(src) + 1, "String");
    strcpy(dest, src);
    return dest;
}

int8_t my_callback(j65_parser *p, uint8_t event) {
    const char *s;
    CityWeather *cw = j65_get_context(p);

    switch (event) {
    case J65_KEY:
        s = j65_get_string(p);
        switch (current_state) {
        case PARSER_INIT:
            if (!strcmp(s, "main")) {
                current_state = PARSER_GET_WEATHER;
            }
            break;
        case PARSER_EXP_CITY_NAME:
            if (!strcmp(s, "name")) {
                current_state = PARSER_GET_CITY_NAME;
            }
            break;
        }
        break;
    case J65_STRING:
        switch (current_state) {
        case PARSER_GET_WEATHER:
            cw->weather = alloc_cpy(j65_get_string(p));
            current_state = PARSER_GET_DESCRIPTION;
            break;
        case PARSER_GET_DESCRIPTION:
            cw->description = alloc_cpy(j65_get_string(p));
            current_state = PARSER_GET_ICON;
            break;
        case PARSER_GET_ICON:
            cw->icon = alloc_cpy(j65_get_string(p));
            current_state = PARSER_GET_TEMP;
            break;
        case PARSER_GET_CITY_NAME:
            cw->city_name = alloc_cpy(utf8_to_ascii(j65_get_string(p)));
            current_state = PARSER_END;
            break;
        }
        break;
    case J65_NUMBER:
        switch (current_state) {
        case PARSER_GET_TEMP:
            cw->temperatureC = kelvin_to_celsius(str_to_kelvin(j65_get_string(p)));
            cw->temperatureF = celsius_to_fahrenheit(cw->temperatureC);
            current_state = PARSER_GET_FEELS_LIKE;
            break;
        case PARSER_GET_FEELS_LIKE:
            current_state = PARSER_GET_MINIMUM;
            break;
        case PARSER_GET_MINIMUM:
            cw->minimumC = kelvin_to_celsius(str_to_kelvin(j65_get_string(p)));
            cw->minimumF = celsius_to_fahrenheit(cw->minimumC);
            current_state = PARSER_GET_MAXIMUM;
            break;
        case PARSER_GET_MAXIMUM:
            cw->maximumC = kelvin_to_celsius(str_to_kelvin(j65_get_string(p)));
            cw->maximumF = celsius_to_fahrenheit(cw->maximumC);
            current_state = PARSER_EXP_CITY_NAME;
            break;
        }
        break;
    }
    return 0;
}

void parse_api_response(CityWeather *cw, char *buffer, size_t len) {
    j65_parser parser;
    int8_t rc;
    current_state = PARSER_INIT;

    j65_init(&parser, cw, my_callback, MAX_PARSER_DEPTH);
    rc = j65_parse(&parser, buffer, len);
    if (rc != J65_DONE) {
        fail("Parser error rc=%d\n", rc);
    }
}
