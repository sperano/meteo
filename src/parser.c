#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json65-file.h"
#include "json65-tree.h"
#include "config.h"
#include "parser.h"
#include "utils.h"

#define SCRATCH_SIZE 1024

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
    char *dest = safe_malloc(strlen(src) + 1);
    strcpy(dest, src);
    return dest;
}


int8_t __fastcall__ my_callback(j65_parser *p, uint8_t event) {
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
            cw->city_name = alloc_cpy(j65_get_string(p));
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

#define MAX_DEPTH 5
void parse_api_response(CityWeather *cw, FILE *f) {
    j65_parser parser;
    uint16_t len = 0;
    int8_t rc;
    char *scratch = safe_malloc(SCRATCH_SIZE);
    current_state = PARSER_INIT;

    do {
        scratch[len] = fgetc(f);
        if (feof(f)) {
            break;
        }
        len++;
        if (len == SCRATCH_SIZE) {
            fail("JSON File too big");
        }
    } while(1);
    j65_init(&parser, cw, my_callback, MAX_DEPTH);
    rc = j65_parse(&parser, scratch, len);
    if (rc != J65_DONE) {
        printf("rc=%d\n", rc);
        fail("parser error");
    }
    free(scratch);
}

/*
int parse_api_response(CityWeather *cw, FILE *f) {
    j65_tree tree;
    int8_t ret;
    j65_node *name, *weather_obj, *weather, *description, *icon, *main,
        *temperature, *minimum, *maximum, *humidity;
    uint8_t *scratch;

    j65_init_tree(&tree);
    scratch = safe_malloc(SCRATCH_SIZE);
    ret = j65_parse_file(f,                // file to parse
                        scratch,           // pointer to a scratch buffer
                        SCRATCH_SIZE,      // length of scratch buffer
                        //sizeof (scratch),  // length of scratch buffer
                        &tree,             // "context" for callback
                        j65_tree_callback, // the callback function
                        0,                 // 0 means use max nesting depth
                        stderr,            // where to print errors
                        40,                // width of screen (for errors)
                        "weather.json",    // used in error messages
                        NULL);             // no custom error func
    if (ret < 0) {
        fail("Error parsing JSON");
    }
    name = j65_find_key(&tree, tree.root, "name");
    if (name == NULL) {
        fail("Can't find key 'name'");
    }
    cw->city_name = alloc_cpy(name->child->string);
    weather_obj = j65_find_key(&tree, tree.root, "weather");
    if (weather_obj == NULL) {
        fail("Can't find key 'weather'");
    }
    main = j65_find_key(&tree, tree.root, "main");
    if (main == NULL) {
        fail("Can't find key 'main'");
    }
    weather = weather_obj->child->child->child->next;
    description = weather->next;
    icon = description->next;
    temperature = main->child->child; //child->child->next;
    minimum = temperature->next->next;
    maximum = minimum->next;
    //humidity = maximum->next->next;
    //printf("TEMPERATURE=%s\n", temperature->child->string);
    cw->weather = alloc_cpy(weather->child->string);
    cw->description = alloc_cpy(description->child->string);
    cw->icon = alloc_cpy(icon->child->string);
    cw->temperatureC = kelvin_to_celsius(str_to_kelvin(temperature->child->string));
    cw->temperatureF = celsius_to_fahrenheit(cw->temperatureC);
    cw->minimumC = kelvin_to_celsius(str_to_kelvin(minimum->child->string));
    cw->minimumF = celsius_to_fahrenheit(cw->minimumC);
    cw->maximumC = kelvin_to_celsius(str_to_kelvin(maximum->child->string));
    cw->maximumF = celsius_to_fahrenheit(cw->maximumC);
    //cw->humidity = str_to_int(humidity->child->string);

    j65_free_tree(&tree);
    free(scratch);
    return 0;
}
*/
