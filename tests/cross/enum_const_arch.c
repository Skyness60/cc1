/* Cross-compilation test: enum constants architecture-dependent */

enum e {
    A = ~(unsigned long int)1 % 7
};

/* On i386: unsigned long is 32-bit, so ~1U = 0xFFFFFFFE, % 7 = 2 */
/* On x86_64: unsigned long is 64-bit, so ~1UL = 0xFFFFFFFFFFFFFFFE, % 7 = 0 */

int get_a(void) {
    return A;
}
