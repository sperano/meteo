#include <ctype.h>
#include <stdlib.h>
#include "ip65.h"
#include "config.h"
#include "net.h"
#include "parser.h"
#include "utils.h"

MeteoState init_ethernet() {
    return ip65_init(config.ethernet_slot) ? EthernetInitFailed : OK;
}

MeteoState init_dhcp() {
    return dhcp_init() ? DHCPInitFailed : OK;
}

void get_ip_addr(char *buffer) {
    unsigned char *ip_bytes = (unsigned char *)&cfg_ip;
    snprintf(buffer, IP_ADDR_STR_LENGTH, "%d.%d.%d.%d\n", ip_bytes[0], ip_bytes[1], ip_bytes[2], ip_bytes[3]);
}

int16_t download_weather_data(CityWeather *cw) {
    //static char url[128]; // must be 1460 bytes
    char *url = safe_malloc(1460);
    char *buffer = safe_malloc(BUFFER_SIZE);
    int16_t len = 0;
    char *ptr = url;

    sprintf(url, "http://api.openweathermap.org/data/2.5/weather?id=%s&appid=%s", cw->id, config.api_key);
    while (*ptr) {
        *ptr = toascii(*ptr);
        ++ptr;
    }
    len = url_download(url, buffer, BUFFER_SIZE);
    free(url);
    ptr = buffer;
    // go to beginning of json
    while (*ptr && *ptr != '{') {
        ++ptr;
    }
    if (*ptr == 0) {
        return -1; // never found valid json
    }
    parse_api_response(cw, ptr, len-(ptr-buffer));
    free(buffer);
    return len;
}
