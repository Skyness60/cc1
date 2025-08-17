typedef int I;
I g;

int f(I a, I (*cb)(I)) {
    I x = (I)42;
    {
        typedef char I; /* shadow typedef in inner scope */
        I c = 'a';
        (void)c;
    }
    return a;
}

typedef struct S S;
struct S { int a; };

S make(void) {
    S s;
    return s;
}
