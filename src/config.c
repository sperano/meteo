#include "config.h"
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 60

char *trim_whitespace(char *str) {
  char *end;
  while(isspace((unsigned char)*str)) {
    str++;
  }
  if(*str == 0) {
    return str;
  }
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) {
      end--;
  }
  end[1] = '\0';
  return str;
}

MeteoConfig* get_config() {
    FILE *file;
    char line[MAX_LINE_LENGTH] = {0};
    char *pline; //, *ptoken;
    static MeteoConfig config;

    memset(&config, 0, sizeof(config));
    file = fopen(METEO_CONFIG_FILENAME, "r");
    if (file == NULL) {
        printf("Config file does not exists: %s\n", METEO_CONFIG_FILENAME);
    } else {
        printf(">>> Loading config: %s\n", METEO_CONFIG_FILENAME);
        // Get each line until there are none left
        while (fgets(line, MAX_LINE_LENGTH, file)) {
            line[strlen(line)-1] = 0; // remove \n
            pline = trim_whitespace(line);
            if (strlen(pline) > 0) {
                char *ptr_token;
                if (pline[0] == '#') {
                    continue;
                }
                ptr_token = strchr(pline, '=');
                if (ptr_token == NULL) {
                    printf("Invalid config line: \"%s\"\n", pline);
                    exit(10);
                } else {
                    *ptr_token = 0;
                    ptr_token++;
                    if (strcmp("app_id", pline) == 0) {
                        strcpy(config.app_id, ptr_token);
                    }
                    if (strcmp("city0_id", pline) == 0) {
                        strcpy(config.city_ids[0], ptr_token);
                    }
                    if (strcmp("city1_id", pline) == 0) {
                        strcpy(config.city_ids[1], ptr_token);
                    }
                    if (strcmp("city2_id", pline) == 0) {
                        strcpy(config.city_ids[2], ptr_token);
                    }
                    if (strcmp("city3_id", pline) == 0) {
                        strcpy(config.city_ids[3], ptr_token);
                    }
                    if (strcmp("city4_id", pline) == 0) {
                        strcpy(config.city_ids[4], ptr_token);
                    }
                }
            }
        }
    }
    fclose(file);
    return &config;
}

void print_config(MeteoConfig* cfg) {
    uint8_t i;
    printf("AppID: %s\n", cfg->app_id);
    for (i = 0; i < MAX_CITIES; ++i) {
        if (cfg->city_ids[i][0]) {
            printf("CityID[%d]: %s\n", i, cfg->city_ids[i]);
        }
    }
}

void validate_config(MeteoConfig* cfg) {
    uint8_t at_least_one = 0;
    uint8_t i;
    if (strlen(cfg->app_id) != 32) {
        printf("Expected a string of 32 characters for app_id, got %d\n", strlen(cfg->app_id));
        exit(100);
    }
    for (i = 0; i < MAX_CITIES && !at_least_one; ++i) {
        if (strlen(cfg->city_ids[i]) > 0) {
            at_least_one = 1;
        }
    }
    if (!at_least_one) {
        printf("No cities configured.\n");
        exit(101);
    }
}
