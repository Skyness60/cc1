int foo(int, double);

void test(void) {
    int *p = 0;
    foo(p, 3.14); /* p not assignable to int */
}
