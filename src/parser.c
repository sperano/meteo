#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json65-file.h"
#include "json65-tree.h"
#include "config.h"
#include "parser.h"
#include "utils.h"

#define SCRATCH_SIZE 1024

/*
#define FIND_KEY_ERROR(name, treeptr, scratch) printf("Could not find %s.\n", name);\
    j65_free_tree(treeptr);\
    free(scratch);
*/

char*  alloc_cpy(const char *src) {
    char *dest = safe_malloc(strlen(src) + 1);
    strcpy(dest, src);
    return dest;
}

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
    cw->city_name = alloc_cpy(name->child->string);
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
