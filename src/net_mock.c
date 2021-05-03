#include <stdio.h>
#include "net.h"
#include "parser.h"
#include "utils.h"

void init_net(void) {
}

CityWeather* download_weather_data(char *city_id) {
    static char buffer[SCRATCH_SIZE];
    uint16_t len = 0;
    char filename[15];
    FILE *f;
    CityWeather *cw;

    sprintf(filename, "W%s.JSON", city_id);
    printf("Loading %s\n", filename);
    f = fopen(filename, "r");
    if (!f) {
        fail("Can't open %s\n", filename);
        return NULL;
    }
    do {
        buffer[len] = fgetc(f);
        if (feof(f)) {
            break;
        }
        len++;
        if (len == SCRATCH_SIZE) {
            fail("JSON File too big\n");
        }
    } while(1);
    buffer[len] = 0;

    cw = safe_malloc(sizeof(CityWeather), "CityWeather");
    cw->id = city_id;
    printf("Read %d bytes.\n", len);
    parse_api_response(cw, buffer, len);
    fclose(f);
    return cw;
}
