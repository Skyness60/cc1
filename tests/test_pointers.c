int add(int a, int b) {
    return a + b;
}

int main() {
    int x = 10;
    int *ptr = &x;
    int value = *ptr;
    
    int array[5] = {1, 2, 3, 4, 5};
    int first = array[0];
    
    int sum = add(x, value);
    
    return 0;
}
