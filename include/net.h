#ifndef _NET_H
#define _NET_H

#include "config.h"
#include "types.h"

#define IP_ADDR_STR_LENGTH 16 // XXX.XXX.XXX.XXX (4*3)+3+NUL

MeteoState init_ethernet();
MeteoState init_dhcp();
void get_ip_addr(char *buffer);
bool download_weather_data(CityWeather *city_weather);

#endif
