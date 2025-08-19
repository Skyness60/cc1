/* Switch case in C89 */
#include <stdio.h>

int main(void) {
    int x = 2;
    switch (x) {
        case 1:
            printf("One\n");
            break;
        case 2:
            printf("Two\n");
            break;
        default:
            printf("Other\n");
            break;
    }
    return 0;
}
