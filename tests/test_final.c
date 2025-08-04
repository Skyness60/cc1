/* Compilateur C89 complet */
#include <stdio.h>

typedef struct {
    int x, y;
} Point;

int main() {
    unsigned long count = 0xFFUL;
    float pi = 3.14159f;
    double e = 2.71828;
    char c = 'A';
    
    Point p = {10, 20};
    int array[5] = {1, 2, 3, 4, 5};
    
    for (int i = 0; i < 5; i++) {
        array[i] *= 2;
        count += array[i];
    }
    
    if (count > 100UL) {
        printf("Count: %lu\n", count);
    }
    
    return 0;
}
