static void NickelMain(void) {
    int *framebuffer = (int *)0x80000000;
    for (int i = 0; i < 320 * 240; ++i) {
        framebuffer[i] = 0x0000FF96;
    }
    while (1);
}