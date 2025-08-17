extern int eg;
static int sg;

int f(const int *p, volatile int *q) {
    int a;
    const int b = 0;
    volatile int c;
    (void)p; (void)q; (void)a; (void)b; (void)c;
    return 0;
}
