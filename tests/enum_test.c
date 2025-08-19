/* Enum usage in C89 */
#include <stdio.h>

enum Color {
    RED,
    GREEN,
    BLUE
};

int main(void) {
    enum Color c = GREEN;
    printf("Color: %d\n", c);
    return 0;
}
