static void NickelMain(void);

static void NickelMain(void) {
    int ret = 0, i;
    for (i = 0; i < 50; ++i) {
        ret *= i;
    }
    i = ret;
    while (1);
}