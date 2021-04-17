#include <assert.h>
#include "utils.h"

int test_get_int1(void) {
    assert(get_int("123") == 123);
    assert(get_int("123.4") == 123);
    assert(get_int("123.5") == 124);
    assert(get_int("123.6") == 124);
    return 0;
}

int test_get_int2(void) {
    return get_int("123.4") - 123;
}
