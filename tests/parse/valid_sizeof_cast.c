int f(void) {
    int x = 1;
    int y = (int) (x + 2);
    int s1 = sizeof x;
    int s2 = sizeof(int);
    return y + s1 + s2;
}
