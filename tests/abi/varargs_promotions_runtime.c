#include <stdio.h>
#include <stdarg.h>

static int sum_ints(int n, ...) {
    va_list ap; va_start(ap, n);
    int s = 0;
    while (n--) s += va_arg(ap, int);
    va_end(ap);
    return s;
}

static int sum_mixed(int tag, ...) {
    va_list ap; va_start(ap, tag);
    int res = 0;
    if (tag == 1) {
        int a = va_arg(ap, int);
        int b = va_arg(ap, int);
        res = a + b;
    } else if (tag == 2) {
        double d = va_arg(ap, double);
        int i = va_arg(ap, int);
        res = (int)d + i;
    } else if (tag == 3) {
        char c = (char)va_arg(ap, int); /* default arg promotions */
        short s = (short)va_arg(ap, int);
        res = (int)c + (int)s;
    }
    va_end(ap);
    return res;
}

int main(void) {
    int a = sum_ints(4, 1, 2, 3, 4);
    int b = sum_mixed(1, 10, 20);
    int c = sum_mixed(2, 3.5, 7);
    int d = sum_mixed(3, (char)5, (short)6);
    printf("%d %d %d %d\n", a, b, c, d);
    return 0;
}
