int f(int a, char *s) {
    int x = 0;
    if (a) x = 1; else x = 2;
    while (x < 10) x = x + 1;
    do x = x - 1; while (x > 0);
    for (x = 0; x < 3; x = x + 1) ;
    return x;
}
