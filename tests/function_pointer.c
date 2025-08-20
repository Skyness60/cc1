/* Function pointer in C89 */
#include <stdio.h>

int add(int a, int b) {
    return a + b;
}

int main(void) {
    int (*func)(int, int) = add;
    printf("Result: %d\n", func(2, 3));
    return 0;
}
