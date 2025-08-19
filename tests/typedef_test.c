/* Typedef usage in C89 */
#include <stdio.h>

typedef unsigned int uint;

int main(void) {
    uint x = 42;
    printf("Typedef value: %u\n", x);
    return 0;
}
