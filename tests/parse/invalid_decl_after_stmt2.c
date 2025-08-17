void g(void) {
    int a;
    ++a;
    int b; // invalid: decl after stmt in block (C89)
}
