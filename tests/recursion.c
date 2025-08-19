/* Recursion in C89 */
#include <stdio.h>

int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

int main(void) {
    printf("Factorial 5: %d\n", factorial(5));
    return 0;
}
