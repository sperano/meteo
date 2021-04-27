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
    uint8_t eth_init = 5; //ETH_INIT_DEFAULT;
    char *ptr = url;

    read_config();
    /*
    for (len = 1; len < 8; ++len) {
        printf("Init ether slot #%d\n", len);
        eth_init &= ~'0';
        if (ip65_init(len)) {
            //printf("failed\n");
            printf("- %s\n", ip65_strerror(ip65_error));
        } else {
            printf("OK!\n");
        }
    }
    */
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
    len = url_download(url, download, sizeof(download));

    printf("len=%d\n%s\n", len, download);
}
