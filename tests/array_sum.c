/* Array sum in C89 */
#include <stdio.h>

int main(void) {
    int arr[5] = {1, 2, 3, 4, 5};
    int sum = 0;
    int i;
    for (i = 0; i < 5; i++) {
        sum += arr[i];
    }
    printf("Sum: %d\n", sum);
    return 0;
}
