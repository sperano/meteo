#ifndef _NET_H
#define _NET_H

#include "types.h"

void init_net(void);
CityWeather* download_weather_data(char *city_id);

#endif
