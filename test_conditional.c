#define DEBUG 1

int main() {
#ifdef DEBUG  
    int debug_var = 42;
#endif
#ifndef NDEBUG
    int another_var = 10;
#endif
#if DEBUG
    return debug_var;
#else
    return 0;
#endif
}
