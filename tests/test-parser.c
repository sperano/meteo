//#include <string.h>

static int do_test (void) {
    return 0; // 1 == failure
}

int main (int argc, char **argv) {
    int badness = 0;

    badness += do_test();

    return badness;
}
