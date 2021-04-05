#include <stdio.h>
#include <stdlib.h>
#include "vendor/json65-master/src/json65-file.h"
#include "vendor/json65-master/src/json65-tree.h"
#include "meteo.h"

static uint8_t scratch[1024];

int parse_api_response(CityWeather *cw, FILE *f) {
    //CityWeather *cw;
    j65_tree tree;
    int8_t ret;
    j65_node *name, *weather_obj, *weather, *description, *icon;

    j65_init_tree(&tree);

    ret = j65_parse_file(f,                // file to parse
                        scratch,           // pointer to a scratch buffer
                        sizeof (scratch),  // length of scratch buffer
                        &tree,             // "context" for callback
                        j65_tree_callback, // the callback function
                        0,                 // 0 means use max nesting depth
                        stderr,            // where to print errors
                        40,                // width of screen (for errors)
                        "weather.json",    // used in error messages
                        NULL);             // no custom error func
    if (ret < 0) {
        fclose(f);
        printf("Error parsing JSON: %d", ret);
        return 1;
    }
    name = j65_find_key(&tree, tree.root, "name");
    if (name == NULL) {
        printf("Could not find name.\n");
        j65_free_tree(&tree);
        return 2;
    }
    weather_obj = j65_find_key(&tree, tree.root, "weather");
    if (weather_obj == NULL) {
        printf("Could not find weather.\n");
        j65_free_tree(&tree);
        return 3;
    }
    weather = weather_obj->child->child->child->next;
    description = weather->next;
    icon = description->next;
    //cw = (CityWeather*)malloc(sizeof(CityWeather));
    cw->city_name = name->child->string;
    cw->weather = weather->child->string;
    cw->description = description->child->string;
    cw->icon = icon->child->string;
    j65_free_tree(&tree);
    return 0;
}
