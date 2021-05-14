#include <stdio.h>
#include "test.h"

int main(void) {
    int i = 0;
    void (*tests[MAX_TESTS])(void) = {
        test_str_to_int,
        test_str_to_kelvin,
        test_kelvin_to_celsius,
        test_celsius_to_fahrenheit,
        test_celsius_str,
        test_utf8_to_ascii,
        test_init_config,
        test_clone_config,
        //test_prepare_text,
    };

    for (i = 0; i < MAX_TESTS; ++i) {
        tests[i]();
        printf("Test #%d passed\n", i + 1);
    }
    return 0;
}
