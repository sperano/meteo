#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "ip65.h"
#include "config.h"
#include "utils.h"

void assert_init_config(MeteoConfig *cfg) {
    assert(cfg->ethernet_slot == ETH_INIT_DEFAULT);
    assert(strlen(cfg->api_key) == 32);
    assert(strcmp("................................", cfg->api_key) == 0);
    assert(cfg->cities == NULL);
    assert(cfg->default_units == Fahrenheit);
    assert(cfg->nb_cities == 0);
    assert(cfg->dirty == false);
}

void test_init_config(void) {
    MeteoConfig *cfg1 = init_config(NULL);
    MeteoConfig cfg2;
    assert_init_config(cfg1);
    free_config(cfg1, true);
    init_config(&cfg2);
    assert_init_config(&cfg2);
    free_config(&cfg2, false);
}

void assert_clone_config(MeteoConfig *cfg1, MeteoConfig *cfg2) {
    assert(cfg2 != cfg1); // not the same pointer
    assert(strcmp(cfg2->api_key, "abcdef") == 0);
    assert(cfg2->ethernet_slot == 1);
    assert(cfg2->default_units == Celsius);
    assert(cfg2->dirty);
    assert(cfg2->nb_cities == 2);
    assert(cfg2->cities != cfg1->cities); // not the same pointer
    assert(strcmp(cfg2->cities[0].id, "12345") == 0);
    assert(cfg2->cities[0].city_name != cfg1->cities[0].city_name); // not the same pointer
    assert(strcmp(cfg2->cities[0].city_name, "foo1") == 0);
    assert(cfg2->cities[0].description != cfg1->cities[0].description); // not the same pointer
    assert(strcmp(cfg2->cities[0].description, "desc foo1") == 0);
    assert(cfg2->cities[0].icon != cfg1->cities[0].icon); // not the same pointer
    assert(strcmp(cfg2->cities[0].icon, "i10n") == 0);
    assert(cfg2->cities[0].weather != cfg1->cities[0].weather); // not the same pointer
    assert(strcmp(cfg2->cities[0].weather, "cloud") == 0);
    assert(cfg2->cities[0].temperatureC == 1);
    assert(cfg2->cities[0].minimumC == 2);
    assert(cfg2->cities[0].maximumC == 3);
    assert(cfg2->cities[0].temperatureF == 4);
    assert(cfg2->cities[0].minimumF == 5);
    assert(cfg2->cities[0].maximumF == 6);
    assert(cfg2->cities[0].humidity == 7);
    assert(strcmp(cfg2->cities[1].id, "67890") == 0);
    assert(cfg2->cities[1].city_name != cfg1->cities[1].city_name); // not the same pointer
    assert(strcmp(cfg2->cities[1].city_name, "foo2") == 0);
    assert(cfg2->cities[1].description != cfg1->cities[1].description); // not the same pointer
    assert(strcmp(cfg2->cities[1].description, "desc foo2") == 0);
    assert(cfg2->cities[1].icon != cfg1->cities[1].icon); // not the same pointer
    assert(strcmp(cfg2->cities[1].icon, "i10d") == 0);
    assert(cfg2->cities[1].weather != cfg1->cities[1].weather); // not the same pointer
    assert(strcmp(cfg2->cities[1].weather, "sunny") == 0);
    assert(cfg2->cities[1].temperatureC == 8);
    assert(cfg2->cities[1].minimumC == 9);
    assert(cfg2->cities[1].maximumC == 10);
    assert(cfg2->cities[1].temperatureF == 11);
    assert(cfg2->cities[1].minimumF == 12);
    assert(cfg2->cities[1].maximumF == 13);
    assert(cfg2->cities[1].humidity == 14);
}

void test_clone_config(void) {
    MeteoConfig *cfg1 = init_config(NULL);
    MeteoConfig *cfg2;
    MeteoConfig cfg3;

    strcpy(cfg1->api_key, "abcdef");
    cfg1->ethernet_slot = 1;
    cfg1->nb_cities = 2;
    cfg1->default_units = Celsius;
    cfg1->dirty = true;
    cfg1->cities = safe_malloc(cfg1->nb_cities * sizeof(CityWeather));
    strcpy(cfg1->cities[0].id, "12345");
    cfg1->cities[0].city_name = "foo1" ;
    cfg1->cities[0].description = "desc foo1" ;
    cfg1->cities[0].icon = "i10n";
    cfg1->cities[0].temperatureC = 1;
    cfg1->cities[0].minimumC = 2;
    cfg1->cities[0].maximumC = 3;
    cfg1->cities[0].temperatureF = 4;
    cfg1->cities[0].minimumF = 5;
    cfg1->cities[0].maximumF = 6;
    cfg1->cities[0].humidity = 7;
    cfg1->cities[0].weather = "cloud";
    strcpy(cfg1->cities[1].id, "67890");
    cfg1->cities[1].city_name = "foo2" ;
    cfg1->cities[1].description = "desc foo2" ;
    cfg1->cities[1].icon = "i10d";
    cfg1->cities[1].temperatureC = 8;
    cfg1->cities[1].minimumC = 9;
    cfg1->cities[1].maximumC = 10;
    cfg1->cities[1].temperatureF = 11;
    cfg1->cities[1].minimumF = 12;
    cfg1->cities[1].maximumF = 13;
    cfg1->cities[1].humidity = 14;
    cfg1->cities[1].weather = "sunny";

    cfg2 = clone_config(NULL, cfg1);
    assert_clone_config(cfg1, cfg2);
    clone_config(&cfg3, cfg1);
    assert_clone_config(cfg1, cfg2);

    free_config(cfg1, true);
    free_config(cfg2, true);
    free_config(&cfg3, false);
}
