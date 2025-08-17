int h(void) {
    int a = 0;
    a = 1;
    int b = 2; // C89: déclaration après statement interdite
    return a + b;
}
