#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vendor/json65-master/src/json65-file.h"
#include "vendor/json65-master/src/json65-tree.h"
#include "meteo.h"

#define SCRATCH_SIZE 1024

#define FIND_KEY_ERROR(name, treeptr, scratch) printf("Could not find %s.\n", name);\
    j65_free_tree(treeptr);\
    free(scratch);

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
    j65_node *name, *weather_obj, *weather, *description, *icon, *main,
        *temperature, *minimum, *maximum, *humidity;
    uint8_t *scratch;

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
        FIND_KEY_ERROR("name", &tree, scratch);
        return 2;
    }
    weather_obj = j65_find_key(&tree, tree.root, "weather");
    if (weather_obj == NULL) {
        FIND_KEY_ERROR("weather", &tree, scratch);
        return 3;
    }
    main = j65_find_key(&tree, tree.root, "main");
    if (main == NULL) {
        FIND_KEY_ERROR("main", &tree, scratch);
        return 4;
    }
    weather = weather_obj->child->child->child->next;
    description = weather->next;
    icon = description->next;
    temperature = main->child->child; //child->child->next;
    minimum = temperature->next;
    maximum = minimum->next;
    humidity = maximum->next;
    //printf("TEMPERATURE=%s\n", temperature->child->string);
    //cw = (CityWeather*)malloc(sizeof(CityWeather));
    cw->city_name = alloc_cpy(name->child->string);
    cw->weather = alloc_cpy(weather->child->string);
    cw->description = alloc_cpy(description->child->string);
    cw->icon = alloc_cpy(icon->child->string);
    cw->temperature = get_int(temperature->child->string);
    cw->minimum = get_int(minimum->child->string);
    cw->maximum = get_int(maximum->child->string);
    cw->humidity = get_int(humidity->child->string);

    j65_free_tree(&tree);
    free(scratch);
    return 0;
}
