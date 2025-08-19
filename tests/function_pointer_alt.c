/* Function pointer alternative test */
int add(int a, int b) {
    return a + b;
}

int main(void) {
    int (*func_ptr)(int, int);
    func_ptr = add;
    return func_ptr(2, 3);
}
