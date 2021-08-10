#include <stdio.h>
#include <string.h>
#include "net.h"
#include "parser.h"
#include "utils.h"

MeteoState init_ethernet() {
    //return EthernetInitFailed;
    return OK;
}

MeteoState init_dhcp() {
    //return DHCPInitFailed;
    return OK;
}

void get_ip_addr(char *buffer) {
    strcpy(buffer, "192.168.0.20");
}

int16_t download_weather_data(CityWeather *cw) {
    static char buffer[BUFFER_SIZE];
    uint16_t len = 0;
    char filename[15];
    FILE *f;

    sprintf(filename, "W%s.JSON", cw->id);
    //printf("Loading %s\n", filename);
    f = fopen(filename, "r");
    if (!f) {
        //fail("Can't open %s\n", filename);
        return -1;
    }
    do {
        buffer[len] = fgetc(f);
        if (feof(f)) {
            break;
        }
        len++;
        if (len == BUFFER_SIZE) {
            fail(FailJSONTooLarge, 0);
        }
    } while(1);
    buffer[len] = 0;
    //printf("Read %d bytes.\n", len);
    parse_api_response(cw, buffer, len);
    fclose(f);
    return len;
}
