#include <stdio.h>

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MSG "Hello from macro"

int main() {
    int x = 5, y = 10;
    printf("%s\n", MSG);
    printf("Max of %d and %d is %d\n", x, y, MAX(x, y));
    return 0;
}
