#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "ip65.h"
#include "config.h"
#include "utils.h"

static char url[2048];
static char download[2048];

void main(void) {
    uint16_t len;
    uint8_t eth_init = ETH_INIT_DEFAULT;
    char *ptr = url;

    read_config();

    if (ip65_init(eth_init)) {
        fail("Error initializing ethernet");
    }
    printf("- Ok\n\nObtaining IP address ");
    if (dhcp_init()) {
        fail("Error DHCP");
    }
    strcpy(url, "http://api.openweathermap.org/data/2.5/weather?id=5368335&appid=f3b3e0e41592b6706cc8b6040a335f45");

    while (*ptr) {
        *ptr = toascii(*ptr);
        ++ptr;
    }
    if (strlen(url) > 1400) {
        ip65_error = IP65_ERROR_MALFORMED_URL;
        fail("IP65_ERROR_MALFORMED_URL");
    }
    len = url_download(url, download, sizeof(download));

    printf("len=%d\n%s\n", len, download);
}
