#ifndef _PARSER_H
#define _PARSER_H

#include <stdio.h>
#include "types.h"

#define MAX_PARSER_DEPTH 5

void parse_api_response(CityWeather *cw, char *buffer, size_t len);

#endif
