int f(void) {
    sizeof(int (*)());
    sizeof(int (*)[10]);
    (void)(int (*)())0;
    (void)(int (*)(int))0;
    return 0;
}
