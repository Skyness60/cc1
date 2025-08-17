void v(int n, ...);

void f() {
    float x = 1.0;   /* default promotions: float->double */
    short s = 1;     /* short->int */
    char c = 2;      /* char->int */
    v(3, x, s, c);
}
