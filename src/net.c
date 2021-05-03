#include <ctype.h>
#include <stdlib.h>
#include "ip65.h"
#include "config.h"
#include "net.h"
#include "parser.h"
#include "utils.h"

void init_net(void) {
    unsigned char *ip_bytes = (unsigned char *)&cfg_ip;
    if (ip65_init(config.ethernet_slot)) {
        fail("Error initializing ethernet\n");
    }
    printf("Obtaining IP address...\n");
    if (dhcp_init()) {
        fail("Error initializing DHCP\n");
    }
    printf("IP: %d.%d.%d.%d\n", ip_bytes[0], ip_bytes[1], ip_bytes[2], ip_bytes[3]);
}

CityWeather* download_weather_data(char *city_id) {
    CityWeather *cw;
    //static char url[128]; // must be 1460 bytes
    char *url = safe_malloc(1460, "URL");
    char *buffer = safe_malloc(SCRATCH_SIZE, "Scratch");
    uint16_t len = 0;
    char *ptr = url;

    sprintf(url, "http://api.openweathermap.org/data/2.5/weather?id=%s&appid=%s", city_id, config.app_id);

    while (*ptr) {
        *ptr = toascii(*ptr);
        ++ptr;
    }
    printf("Downloading weather for %s\n", city_id);
    len = url_download(url, buffer, SCRATCH_SIZE);
    free(url);
    printf("Downloaded %d bytes.\n", len);
    ptr = buffer;
    // go to beginning of json
    while (*ptr && *ptr != '{') {
        ++ptr; // TODO assert that we found one {}
    }
    cw = safe_malloc(sizeof(CityWeather), "CityWeather");
    cw->id = city_id;
    parse_api_response(cw, ptr, len-(ptr-buffer));
    free(buffer);
    return cw;
}
