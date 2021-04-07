#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vendor/json65-master/src/json65-file.h"
#include "vendor/json65-master/src/json65-tree.h"
#include "meteo.h"

#define SCRATCH_SIZE 1024

//static uint8_t scratch[1024];

char* alloc_cpy(const char *src) {
    char *dest = malloc(strlen(src) + 1);
    if (dest == NULL) {
        printf("Out of memory");
        exit(5);
    }
    strcpy(dest, src);
    return dest;
}

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

int parse_api_response(CityWeather *cw, FILE *f) {
    j65_tree tree;
    int8_t ret;
    j65_node *name, *weather_obj, *weather, *description, *icon;
    uint8_t *scratch;
    //char *ptr;

    j65_init_tree(&tree);
    scratch = malloc(SCRATCH_SIZE);
    if (scratch == NULL) {
        printf("Not enough memory for scratch\n");
    }

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
        printf("Error parsing JSON: %d", ret);
        fclose(f);
        free(scratch);
        return 1;
    }
    name = j65_find_key(&tree, tree.root, "name");
    if (name == NULL) {
        printf("Could not find name.\n");
        j65_free_tree(&tree);
        free(scratch);
        return 2;
    }
    weather_obj = j65_find_key(&tree, tree.root, "weather");
    if (weather_obj == NULL) {
        printf("Could not find weather.\n");
        j65_free_tree(&tree);
        free(scratch);
        return 3;
    }
    weather = weather_obj->child->child->child->next;
    description = weather->next;
    icon = description->next;
    //cw = (CityWeather*)malloc(sizeof(CityWeather));
    /*
    cw->city_name = ;
    cw->weather = weather->child->string;
    cw->description = description->child->string;
    cw->icon = icon->child->string;
    */
    cw->city_name = alloc_cpy(name->child->string);
    cw->weather = alloc_cpy(weather->child->string);
    cw->description = alloc_cpy(description->child->string);
    cw->icon = alloc_cpy(icon->child->string);
    cw->temperature = get_int("37.5");
    cw->minimum = get_int("37.4");
    cw->maximum = get_int("38.7");
    cw->humidity = get_int("25");

    j65_free_tree(&tree);
    free(scratch);
    return 0;
}
