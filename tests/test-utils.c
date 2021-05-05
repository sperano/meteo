#include <assert.h>
#include "utils.h"
#include <stdio.h>
#include <string.h>

void test_str_to_int(void) {
    assert(str_to_int("123") == 123);
    assert(str_to_int("123.4") == 123);
    assert(str_to_int("123.5") == 124);
    assert(str_to_int("123.6") == 124);
}

void test_str_to_kelvin(void) {
    assert(str_to_kelvin("223") == 22300);
    assert(str_to_kelvin("223.45") == 22345);
    assert(str_to_kelvin("223.8") == 22380);
}

void test_kelvin_to_celsius(void) {
    assert(kelvin_to_celsius(29315) == 200); // 293.15K = 20.0 C
    assert(kelvin_to_celsius(29355) == 204); // 293.55K = 20.4 C
}

void test_celsius_to_fahrenheit(void) {
    assert(celsius_to_fahrenheit(200) == 68); // 20.0C = 68F
    assert(celsius_to_fahrenheit(104) == 51); // 10.4C = 50.72F
}

void test_celsius_str(void) {
    char buffer[5];
    celsius_str(buffer, 372);
    assert(!strcmp("37.2", buffer));
    celsius_str(buffer, 200);
    assert(!strcmp("20", buffer));
}

void test_utf8_to_ascii(void) {
    char str1[] = {'M', 'o', 'n', 't', 'r', 0xc3, 0xa9,'a', 'l', 0};
    char str2[] = {'M', 'o', 'n', 't', 'r', 0xc3, 0xa8,'a', 'l', 0};

    assert(!strcmp("Montreal", utf8_to_ascii("Montreal")));
    //printf("%s\n", utf8_to_ascii(str1));
    assert(!strcmp("Montreal", utf8_to_ascii(str1)));
    assert(!strcmp(str2, utf8_to_ascii(str2)));
}

/*
void test_prepare_text(void) {
    CityWeather cw;
    cw.city_name = "Los Altos";
    cw.weather = "Cloudy";
    cw.description = "some clouds";
    cw.temperatureC=244;
    cw.minimumC=221;
    cw.maximumC=273;
    cw.temperatureF = celsius_to_fahrenheit(cw.temperatureC);
    cw.minimumF = celsius_to_fahrenheit(cw.minimumC);
    cw.maximumF = celsius_to_fahrenheit(cw.maximumC);

    prepare_text(&cw);
    //printf("76F          72F          81F           !\n");
    //printf("%s!\n", cw.text_lines[3]);
    assert(!strcmp("Los Altos: Cloudy (some clouds)         ", cw.text_lines[0]));
    assert(!strcmp("24.4C  /  Min: 22.1C  /  Max: 27.3C     ", cw.text_lines[1]));
    assert(!strcmp("                                        ", cw.text_lines[2]));
    //assert(!strcmp("C:Configure | U:Unit | Q:Quit           ", cw.text_lines[3]));

}
*/
