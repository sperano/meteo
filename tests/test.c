#include <stdio.h>
#include "test.h"

int (*func_ptrs[MAX_TESTS])(void) = {
    test_get_int1,
    test_get_int2,
};

int main(void) {
    int i = 0;
    int rc;

    for (i = 0; i < MAX_TESTS; ++i) {
        rc = func_ptrs[i]();
        if (rc != 0) {
            printf("Test #%d failed with rc=%d\n", i + 1, rc);
            return rc;
        }
        printf("Test #%d passed\n", i + 1);
    }
    return 0;
}
