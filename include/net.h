#ifndef _NET_H
#define _NET_H

#include "config.h"
#include "types.h"

#define IP_ADDR_STR_LENGTH 16 // XXX.XXX.XXX.XXX (4*3)+3+NUL

MeteoState init_ethernet(MeteoConfig *config);
MeteoState init_dhcp(MeteoConfig *config);
void get_ip_addr(char *buffer);
void download_weather_data(char *api_key, CityWeather *city_weather);

#endif
